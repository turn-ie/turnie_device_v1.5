#!/usr/bin/env python3
# =============================================================
# turnie 受信履歴(Inbox) 一括吸い出しツール（WiFi / telnet 経由・読み取り専用）
#
#   同一WiFi上で OTAモード（緑LED）になっている turnie 全台を
#   mDNS（_arduino._tcp）で自動探索し、各台に telnet(23) で接続して
#   "dump" コマンドを実行、受信履歴(JSON)をPCに保存する。
#
#   ※ 読み取り専用：デバイスへの書き込み・消去は一切行わない。
#   ※ OTA一斉書き込み(upload_ota.py)とは独立。OTA機能には触れない。
#
#   前提条件（OTA一斉書き込みと同じ）:
#     - 各デバイスを BOOT ボタン短押しで OTAモード（緑LED）にしておく
#     - Mac がデバイスと同じWiFiに接続されている
#
#   使い方:
#     python3 dump_all.py            # 自動探索して全台吸い出し
#     python3 dump_all.py --host 192.168.10.23   # 特定の1台だけ（IP指定）
# =============================================================
import argparse
import json
import os
import socket
import subprocess
import sys
import time
from datetime import datetime

BEGIN_MARKER = "===TURNIE_INBOX_BEGIN==="
END_MARKER = "===TURNIE_INBOX_END==="
TELNET_PORT = 23


def run_dns_sd(args, timeout):
    """dns-sd は自分で終了しないので timeout で打ち切り、得られた出力を返す。"""
    try:
        p = subprocess.run(["dns-sd"] + args, capture_output=True, text=True, timeout=timeout)
        return p.stdout
    except subprocess.TimeoutExpired as e:
        return e.stdout.decode() if isinstance(e.stdout, bytes) else (e.stdout or "")
    except FileNotFoundError:
        print("[!] dns-sd が見つかりません（macOS標準）。--host でIP指定してください。")
        return ""


def discover():
    """OTAモードの turnie 全台を探索し、(名前, IP) のリストを返す。

    service広告(_arduino._tcp)を引いたあと各台のIPを解決し、
    解決できた台だけを対象にする（古いmDNS広告＝オフライン機は自然に除外される）。
    """
    print("[*] mDNS(_arduino._tcp) で OTAモード(緑LED)のデバイスを探索中...")
    names = []
    for line in run_dns_sd(["-B", "_arduino._tcp"], timeout=5).splitlines():
        if "Add" in line and "_arduino._tcp." in line:
            n = line.split("_arduino._tcp.")[-1].strip()
            if n and n not in names:
                names.append(n)

    devices = []
    for n in names:
        host = n + ".local"
        ip = None
        for line in run_dns_sd(["-G", "v4", host], timeout=4).splitlines():
            if "Add" in line and host in line:
                for tok in line.split():
                    if tok.count(".") == 3 and all(p.isdigit() for p in tok.split(".")):
                        ip = tok
        if ip:
            devices.append((n, ip))
        else:
            print(f"    [-] {n}: IP解決不能（オフラインの可能性）→ 除外")
    return devices


def fetch_inbox(ip):
    """1台に telnet 接続して dump を実行し、マーカー間の生テキストを返す。失敗時 None。"""
    try:
        s = socket.create_connection((ip, TELNET_PORT), timeout=6)
    except Exception as e:
        print(f"    [!] 接続失敗 ({ip}): {e}")
        return None
    s.settimeout(10)
    try:
        time.sleep(0.4)
        try:
            s.recv(2048)  # ウェルカムバナーを読み捨てる
        except socket.timeout:
            pass
        s.sendall(b"dump\n")
        buf = b""
        while END_MARKER.encode() not in buf:
            try:
                chunk = s.recv(8192)
            except socket.timeout:
                break
            if not chunk:
                break
            buf += chunk
    finally:
        s.close()

    b = buf.find(BEGIN_MARKER.encode())
    e = buf.find(END_MARKER.encode())
    if b < 0 or e < 0:
        return None
    return buf[b + len(BEGIN_MARKER):e].strip(b"\r\n").decode("utf-8", "replace")


def parse_inbox(text):
    """JSONに紛れた生制御文字を \\uXXXX にエスケープしてパースする。失敗時 None。"""
    safe = "".join(c if ord(c) >= 0x20 or c == "\t" else "\\u%04x" % ord(c) for c in text)
    try:
        return json.loads(safe)
    except json.JSONDecodeError:
        return None


def main():
    ap = argparse.ArgumentParser(description="turnie 受信履歴を同一WiFiの全台から一括吸い出し（読み取り専用）")
    ap.add_argument("--host", help="特定の1台だけ吸い出す（IP指定。例: 192.168.10.23）")
    ap.add_argument("--outdir", default=".", help="保存先フォルダ（デフォルト: カレント）")
    args = ap.parse_args()

    print("=== turnie Inbox 一括吸い出しツール（WiFi/telnet・読み取り専用）===")
    os.makedirs(args.outdir, exist_ok=True)

    if args.host:
        devices = [(args.host, args.host)]
    else:
        devices = discover()
        if not devices:
            print("\n[!] OTAモード(緑LED)のデバイスが見つかりませんでした。")
            print("    BOOTボタン短押しで緑LEDにして、Macが同じWiFiにいるか確認してください。")
            sys.exit(1)
        print(f"[+] {len(devices)} 台発見: " + ", ".join(f"{n}({ip})" for n, ip in devices))

    stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    success = 0
    for name, ip in devices:
        print(f"\n--- {name} ({ip}) ---")
        text = fetch_inbox(ip)
        if text is None:
            print("    [!] dump 取得失敗")
            continue
        data = parse_inbox(text)
        if data is None:
            print("    [!] JSONパース不能")
            continue
        out = os.path.join(args.outdir, f"inbox_{name}_{stamp}.json")
        with open(out, "w", encoding="utf-8") as f:
            json.dump(data, f, indent=2, ensure_ascii=False)
        print(f"    [+] {len(data)} 件保存: {out}")
        success += 1

    print(f"\n=== 完了: {success}/{len(devices)} 台成功 ===")
    sys.exit(0 if success == len(devices) else 1)


if __name__ == "__main__":
    main()
