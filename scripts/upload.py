#!/usr/bin/env python3
"""USB 有線書き込みツール（旧 upload.sh の Python 版）。

デバイスの USB シリアル番号（MAC アドレス）から接続中のポートを自動検出し、
対応する PlatformIO env で書き込む。

使い方（プロジェクトルートで実行）:
    python3 scripts/upload.py 1   # Device 1 (env:device1) に書き込み
    python3 scripts/upload.py 2   # Device 2 (env:device2) に書き込み
"""
import subprocess
import sys

# デバイス番号 -> (PlatformIO env, USB シリアル番号 = MAC アドレス)
DEVICES = {
    "1": ("device1", "A0:F2:62:EC:40:B8"),
    "2": ("device2", "94:A9:90:39:3D:28"),
}


def find_port(serial: str):
    """pio device list の出力から、指定シリアル番号に対応するポートを探す。"""
    out = subprocess.run(
        ["pio", "device", "list"], capture_output=True, text=True
    ).stdout
    blocks = out.split("\n\n")
    for block in blocks:
        if serial in block:
            # ブロック先頭行がポート名
            return block.strip().splitlines()[0].strip()
    return None


def main():
    if len(sys.argv) < 2 or sys.argv[1] not in DEVICES:
        print("==== USB 書き込みスクリプト ====")
        print("使い方:")
        for num, (env, ser) in DEVICES.items():
            print(f"  python3 scripts/upload.py {num}  -> {env} (MAC: {ser})")
        sys.exit(1)

    env, serial = DEVICES[sys.argv[1]]
    port = find_port(serial)
    if not port:
        print(f"[!] デバイス (Serial: {serial}) が見つかりません。USB 接続を確認してください。")
        sys.exit(1)

    print(f"[*] {env} (Port: {port}) に書き込みを開始します...")
    rc = subprocess.run(
        ["pio", "run", "-e", env, "-t", "upload", "--upload-port", port]
    ).returncode
    sys.exit(rc)


if __name__ == "__main__":
    main()
