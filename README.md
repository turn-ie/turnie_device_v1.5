# turnie device (v1.5)

ESP32-S3 + 8x8 LED マトリクスの小型デバイス用ファームウェア。
スマホ（BLE GATT）から自分の「コンテンツ（ピクセルアート or 文字）」を設定し、
近くの別の turnie とすれ違うと BLE5 拡張アドバタイズで自動的にコンテンツを交換する。
待機中は IMU の傾きで動く流体シミュレーションを表示する。

---

## ディレクトリ構成（概要）

| ディレクトリ | 役割 |
|------|------|
| ルート（`*.ino` / `*.cpp` / `*.h`） | ファームウェア本体（描画・BLE・コンテンツ管理・OTA など） |
| `scripts/` | 書き込み・受信履歴の吸い出し用 Python ツール |
| `APIreference/` | 参考資料（ESP-IDF BLE / ArduinoJson の API メモ） |

ファイル単位の詳細は [ファイル別構成](#ファイル別構成詳細) を参照。

---

## ハードウェア

| 部品 | 接続 |
|------|------|
| MCU | ESP32-S3 (esp32-s3-devkitc-1) |
| LED | 8x8 WS2812 NeoMatrix … `GPIO14` |
| IMU | QMI8658 (I2C) … `SDA=GPIO11 / SCL=GPIO12` |
| USER ボタン | `GPIO39`（INPUT_PULLUP, 押下=LOW）|
| BOOT ボタン | `GPIO0` |

> **注意:** このパネルは R と G が物理的に入れ替わっています。色順の補正は
> `Display::Internal::color()` の 1 か所に集約しており、全描画パス（画像 / 塗りつぶし /
> 流体 / アニメ）がここを通ります。色がおかしいときはここだけを直せば全体に反映されます。

---

## 主な挙動

### 待機状態（流体シミュレーション）
- IMU の傾きに従って「粒」が動く（`Fluid.cpp`）。
- 粒の数 = 自分のコンテンツ(1) + 受信履歴(Inbox)の件数。
- 粒の色 = そのコンテンツの**代表色**。
  - 画像: 画面全体で**最も多く占める色**（背景の黒は除外）。
  - 文字: 白。
- 通常状態へ戻るときは必ず**フェードイン**する。

### スマホから書き込み（BLE GATT）
1. コンテンツ（画像/文字）を書き込むと、自分のコンテンツとして LittleFS に保存。
2. **3 秒間プレビュー表示**（画像は静止、文字はスクロール）した後、自動で待機へ戻る。
3. 同時に他デバイスへブロードキャスト送信する。
- 設定（`flag:"settings"`）を書き込むと `setting.json` を保存 →
  読み込み直して**新しい設定色で Ripple 演出**を再生する。

### すれ違い受信（BLE5 拡張アドバタイズ）
- 受信すると Ripple 演出 → 受信コンテンツを表示（画像は 1px ずつ点灯）→
  Inbox に保存し、自分のコンテンツを相手へ返信する。
- 連投対策: 一度受信したら `5 秒`は後続を無視。同一内容は `5 分`以内なら拒否。

---

## ボタン操作

デバイスには **USER ボタン**（`GPIO39`）と **BOOT ボタン**（`GPIO0`）の 2 つがある。

### USER ボタン
| 操作 | 通常時 | 閲覧モード中 |
|------|--------|--------------|
| **短押し** | 自分のピクチャ確認モード（10 秒で待機へ自動復帰／もう一度押すと即復帰） | 1 つ古い受信履歴へ進む（最古の次は最新へループ） |
| **長押し**（0.8 秒〜） | 受信履歴の**閲覧モード開始**（DiagonalWave 演出） | 閲覧モード**終了**して待機へ |

- 閲覧モードは無操作が 30 秒（`BROWSE_TIMEOUT_MS`）続くと自動で待機へ戻る。
- 閲覧モードで履歴が無い場合は「no data」を表示する。

### BOOT ボタン
| 操作 | 動作 |
|------|------|
| **短押し**（0.2〜5 秒） | **OTA モード**へ移行（LED 緑）。WiFi 経由の書き込み待ち受け状態になる |
| **長押し**（5 秒〜） | **factory reset**：紫点灯 2 秒 → 全データ（コンテンツ/受信履歴）と設定を初期化 |

> しきい値は `Config.h` の `BROWSE_LONG_PRESS_MS`（長押し判定）/ `BUTTON_DEBOUNCE_MS`（短押しのチャタリング除去）で調整できる。

---

## 期待する JSON 形式

スマホ → デバイス（BLE GATT 書き込み）は以下の JSON。

### 文字
```json
{ "flag": "text", "text": "Hello!" }
```

### 画像（8x8 ピクセルアート）
```json
{ "flag": "emoji", "rgb": [R, G, B, R, G, B, ... 計 192 個] }
```
- `flag` は `"emoji"` / `"photo"` / `"image"` のいずれでも可。
- `rgb` は **8x8 = 64 ピクセル × 3 (R,G,B) = 192 要素**。各 0–255。
- 並びは行優先（左上から右へ、次の行へ）。

### 設定
```json
{
  "flag": "settings",
  "hue": 104,
  "brightness": 50,
  "motion": "Ripple",
  "name": "turnie"
}
```
| キー | 範囲 / 値 | 説明 |
|------|-----------|------|
| `hue` | **0–360**（度） | アニメ・流体の色相 |
| `brightness` | **0–100** | 全体の明るさ（内部で 0–255 へ変換） |
| `motion` | `"Ripple"` / `"DiagonalWave"`（または `0` / `1`） | 演出の種類 |
| `name` | 文字列（最大 31 文字） | BLE デバイス名 |

省略したキーは現在値が維持される。設定は `setting.json` に保存され再起動後も復元される。

---

## ビルドと書き込み

[PlatformIO](https://platformio.org/) を使用。

### 0. 初回セットアップ（認証情報）
WiFi / OTA の認証情報は `Secrets.h`（Git 管理外）に置く。テンプレートからコピーして編集する。
```sh
cp Secrets.h.example Secrets.h
# Secrets.h を開いて WiFi SSID / パスワード等を自分の値に書き換える
```

### 1. USB 有線書き込み
```sh
python3 scripts/upload.py 1   # Device 1 (env:device1)
python3 scripts/upload.py 2   # Device 2 (env:device2)
```
USB シリアル番号(MAC)からポートを自動検出する。対応 MAC は `scripts/upload.py` の `DEVICES` で編集。
直接実行する場合は `pio run -e device1 -t upload`。

### 2. OTA 書き込み（WiFi 経由）
1. デバイスの **BOOT ボタンを短押し**して OTA モードにする（LED が緑色）。
   このとき `Secrets.h` の WiFi に自動接続する。PC も同じ WiFi に接続しておく。
2. 一括書き込み（OTA モードの全台を自動検出）:
   ```sh
   python3 scripts/upload_ota.py
   ```
3. 特定 IP に手動で書き込む場合:
   ```sh
   pio run -e turnie_ota -t upload --upload-port <IPアドレス>
   ```

### シリアルモニタ
```sh
pio device monitor   # 115200 bps
```

---

## ファイル別構成（詳細）

| パス | 内容 |
|------|------|
| `turnie_device_v1.5.ino` | メインループ（ボタン処理・受信処理・状態遷移） |
| `Display.*` | LED マトリクス描画・表示タイマー（色順補正の集約点） |
| `Fluid.*` | 待機画面の流体シミュレーション |
| `Animations.*` | Ripple / DiagonalWave 演出 |
| `Content.*` | コンテンツ（画像/文字）の定義・JSON パース・保存 |
| `Inbox.*` | 受信履歴のリングバッファ |
| `BleBroadcast.*` / `BleGatt.*` / `BleStack.*` | BLE5 ブロードキャスト / スマホ向け GATT |
| `Settings.*` | `setting.json` の読み込み・反映 |
| `OTA_Handler.*` | WiFi OTA |
| `Storage.*` | LittleFS への保存・読み込み |
| `Config.h` | ピン・タイミング等の定数 |
| `Secrets.h` | WiFi/OTA 認証情報（**Git 管理外**。`Secrets.h.example` を参照）|
| `scripts/upload.py` | USB 書き込み |
| `scripts/upload_ota.py` | OTA 一括書き込み |
| `scripts/dump_all.py` | 受信履歴(inbox)の吸い出し |
