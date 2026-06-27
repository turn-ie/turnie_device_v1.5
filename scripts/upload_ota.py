#!/usr/bin/env python3
import socket
import subprocess
import sys
import os
from concurrent.futures import ThreadPoolExecutor

# スキャン対象のポート
# 3232: ArduinoOTA (espota)
# 23: Telnet console (OTA_Handler)
PORT_OTA = 3232
PORT_TELNET = 23

def check_port(ip, port, timeout=1.0):
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(timeout)
            s.connect((ip, port))
            return True
    except Exception:
        return False

def get_local_ip_base():
    try:
        # インターネットへのダミー接続を作って自身のIPアドレスを取得
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        local_ip = s.getsockname()[0]
        s.close()
        # IPv6オンリー環境での仮想IPv4アドレス (192.0.0.x) などを検出した場合は 192.168.10.17 の実績を元に 192.168.10 にフォールバック
        if local_ip.startswith("192.0.0") or local_ip.startswith("127.0.0"):
            return "192.168.10"
        parts = local_ip.split('.')
        if len(parts) == 4:
            return ".".join(parts[:3])
    except Exception:
        pass
    return "192.168.10" # フォールバック


def scan_network(ip_base):
    print(f"[*] ネットワーク {ip_base}.0/24 をスキャン中...")
    found_ips = []

    def worker(i):
        ip = f"{ip_base}.{i}"
        # OTAポート(3232)またはTelnetポート(23)が開いているかチェック
        if check_port(ip, PORT_OTA) or check_port(ip, PORT_TELNET):
            # デバイスのホスト名確認（Telnetからウェルカムメッセージを取得してみる）
            hostname = "Unknown"
            try:
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                    s.settimeout(1.5)
                    s.connect((ip, PORT_TELNET))
                    resp = s.recv(512).decode('utf-8', errors='ignore')
                    # turnie-xxxx.local や IP などの情報が含まれている行を探す
                    for line in resp.split('\n'):
                        if "IP:" in line:
                            hostname = line.strip()
            except Exception:
                pass
            
            found_ips.append((ip, hostname))
            print(f"[+] 発見: {ip} ({hostname})")

    # 20スレッドで少し緩やかにスキャン（ルーターでのパケットドロップを防ぐため）
    with ThreadPoolExecutor(max_workers=20) as executor:
        executor.map(worker, range(1, 255))


    return found_ips

def main():
    # 引数があればそれを使用、なければ自動検出
    if len(sys.argv) > 1:
        ip_base = sys.argv[1]
    else:
        ip_base = get_local_ip_base()

    print(f"=== turnie OTA 一括書き込みツール ===")
    
    # 1. まずビルドを実行してファームウェアを最新にする
    print("\n[*] ファームウェアをビルドしています...")
    build_cmd = ["pio", "run", "-e", "turnie_ota"]
    res = subprocess.run(build_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    if res.returncode != 0:
        print("[!] ビルドに失敗しました。エラーログ:")
        print(res.stdout)
        sys.exit(1)
    print("[+] ビルド成功！")

    # 2. ネットワークスキャン
    found_devices = scan_network(ip_base)
    
    if not found_devices:
        print("\n[!] OTAモード（LEDが緑色）のデバイスが見つかりませんでした。")
        print("    デバイスのBOOTボタンを短く押して、LEDが緑色になっているか確認してください。")
        print(f"    (スキャン対象セグメント: {ip_base}.0/24)")
        sys.exit(1)

    print(f"\n[*] {len(found_devices)} 台のデバイスが見つかりました。書き込みを開始します。")

    # 3. 見つかったデバイスに順次書き込み
    success_count = 0
    for ip, hostname in found_devices:
        print(f"\n------------------------------------------------------------")
        print(f"[*] {ip} ({hostname}) へ書き込み中...")
        print(f"------------------------------------------------------------")
        
        # pio run -e turnie_ota -t upload --upload-port <IP>
        upload_cmd = ["pio", "run", "-e", "turnie_ota", "-t", "upload", "--upload-port", ip]
        
        # リアルタイムで書き込みログを出力
        process = subprocess.Popen(upload_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        
        # ログを逐次出力（進捗バーなどを表示するため）
        while True:
            line = process.stdout.readline()
            if not line and process.poll() is not None:
                break
            if line:
                # 進行状況だけを表示するように少し間引くか、そのまま出力
                sys.stdout.write(line)
                sys.stdout.flush()
                
        rc = process.poll()
        if rc == 0:
            print(f"[+] {ip} への書き込みが成功しました！")
            success_count += 1
        else:
            print(f"[!] {ip} への書き込みに失敗しました。")

    print(f"\n=== 結果 ===")
    print(f" 検出台数: {len(found_devices)} 台")
    print(f" 成功台数: {success_count} 台 / {len(found_devices)} 台")

if __name__ == "__main__":
    main()
