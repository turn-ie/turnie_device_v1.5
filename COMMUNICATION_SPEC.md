# turnie デバイス間通信仕様書 (v1.5)

対象: BleBroadcast.cpp / Config.h

## 1. 概要

各デバイスは BLE 5.0 Extended Advertising (AUX_ADV_IND) を用いて以下を並行実行する。

- **送信 (handle 0)**: tune（JSON または画像）を120ms周期でブロードキャスト
- **受信 (スキャン)**: 他デバイスの広告を常時監視
- **GATT (handle 1)**: スマホからの tune 書き込みを接続受付

Legacy Advertising（最大31B）では約200Bのデータに断片化が必要だが、Extended Advertising は単一パケット（最大255B）で送信でき、「1回の受信＝交換成立」を実現する。

## 2. 物理層設定

| パラメータ | 値 | 備考 |
|---|---|---|
| ADV_INTERVAL | 120 ms (0x00C0 × 0.625ms) | Config.h L56 |
| SCAN_INTERVAL / WINDOW | 50 ms / 30 ms | Duty比 60%。Config.h L60-61 |
| Primary / Secondary PHY | 1M / 1M | スマホ互換性・低消費電力を優先。2M/Codedは不採用 |
| TX Power | 8 dBm | Config.h L57 |

## 3. 広告パケット構造

Manufacturer Specific Data (AD Type 0xFF) を使用。

```
[Length][0xFF][CompID_L][CompID_H][TypeByte][Payload...]
```

- CompanyID = `0xFFFF`（turnie識別用、Config.h L47）
- TypeByte で内容を判別（buildTextPacket / buildImagePacket, L56-95）

### 形式1: テキスト JSON（TypeByte='T'）
- JSON をそのまま UTF-8 で送信（バイナリ変換なし）
- 最大196B（ヘッダ4B + MAX_PAYLOAD 200B以内）

### 形式2: バイナリ画像（TypeByte='I'）
- 8×8 LED の RGB を `[R][G][B]×64 = 192B` のバイナリで送信
- JSONで送ると配列表記のオーバーヘッドで約800Bに膨張し255B制限を超えるため、画像のみバイナリ化して圧縮している

## 4. 2つの広告ハンドル

| | handle 0（デバイス間） | handle 1（GATT/スマホ） |
|---|---|---|
| 種別 | Non-connectable, Non-scannable | Legacy + Connectable + Scannable |
| SID | 0 | 1 |
| 周期 | 120 ms | 100 ms |
| 用途 | tune のブロードキャスト送出 | スマホからの tune 書き込み受付 |

`esp_ble_gap_config_ext_adv_data_raw()` により、アドバタイズを停止せずペイロードを動的更新できる（送信時に都度呼び出し）。

## 5. 受信・重複判定

- スキャンは Passive、Extended Scan（1M PHYのみ、`ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK`）
- 受信データから 0xFF AD構造体を探索し、CompanyID一致でturnie広告と判定（handleReceivedPacket, L167-199）
- RSSIフィルタは `BLE_RSSI_MIN = -128`（現状事実上無効）
- **重複判定は送信元MACではなくtune内容ベース**: 同一内容を `RX_SAME_REJECT_MS`（5分）以内に再受信した場合は破棄する

## 6. 設定値の根拠

- **120ms周期**: すれ違い時間（数秒〜10秒）内に複数回の受信機会を確保しつつ、消費電力とのバランスを取った値。0.625ms単位で192という区切りの良い値
- **Duty 60%（scan_window 30ms / scan_interval 50ms）**: BLE IoTとして一般的な省電力設定
- **1M PHY**: 2Mより低速だがスマホ互換性が高く消費電力も低いため採用。距離面はPHYでは変わらない

## 7. バッファ・制限一覧

| 項目 | 値 |
|---|---|
| MAX_PAYLOAD | 200 B |
| テキストJSON上限 | 196 B |
| 画像RGBデータ | 192 B 固定 |
| Inbox件数 | 30件 |
| RX_SAME_REJECT_MS | 180000 ms (5分) |

## 8. 論文記述用サマリー

> turnie は BLE 5.0 Extended Advertising を採用し、テキスト JSON（最大196B、非バイナリ）またはバイナリ画像（192B固定）を単一の広告パケットで送信する。画像はJSON化すると約800Bに膨張し255B制限を超えるため、RGBバイナリ化により圧縮している。各デバイスは120ms周期の送信と、scan_interval=50ms／scan_window=30ms（Duty 60%）のスキャンを並行実行し、ブロードキャスト・非接続での自動交換を実現する。重複判定は送信元MACではなくtune内容に基づき、5分以内の同一内容再受信を無視する。

---

## 9. システム全体のワークフロー（.ino が統括する状態遷移）

BLE通信はシステムの一部であり、実際の挙動は `turnie_device_v1.5.ino` の `loop()` が全モードを統括している。**明示的な enum ステートマシンは存在せず、フラグ＋タイマー変数の組み合わせ**で以下の状態を切り替える。

### 9.1 状態一覧

| 状態 | 制御フラグ | 概要 |
|---|---|---|
| 待機（流体） | なし（他の全フラグが false の既定状態） | IMUの傾きで動く粒のシミュレーション。粒は「自分のtune」1個＋Inbox件数分 |
| 受信アニメーション→表示 | `newMessageFlag` / `showContentAfterAnim` | tune受信直後、Ripple演出→1ピクセルずつ受信画像/テキストを表示 |
| ピクチャ確認 | `g_previewMode` | USER短押しで自分のtuneを表示、10秒(`PREVIEW_TIMEOUT_MS`)で自動的に待機へ復帰 |
| 閲覧モード | `g_browseMode` | USER長押し(`BROWSE_LONG_PRESS_MS`=800ms)で開始。Inbox履歴を1件ずつ表示、30秒(`BROWSE_TIMEOUT_MS`)無操作で自動終了 |
| OTA/デバッグモード | `isDebugMode()` | BOOTボタン200ms〜1.5秒未満の押下で移行。通常処理は完全にスキップされる |
| リセット | （ワンショット処理） | BOOT長押し1.5秒(`BOOT_RESET_LONG_PRESS_MS`)で紫点灯→全JSONデータ削除→初期値に戻す |

### 9.2 起動シーケンス（`setup()`, L199-239）

```
Display::init() → Storage::init() (LittleFS)
  → Content::init()  … data.json を読み込み、自分のtuneを復元（無ければ Kind::None）
  → Inbox::init()     … 受信履歴をロード
  → Settings::loadFromStorage() … setting.json から hue/brightness/motion/name/hometown を復元
  → BleStack::init() → BleGatt::init()（GATTサービス登録）
  → BleBroadcast::setOnReceive(onMessageReceived)  … 受信コールバック登録
  → 自分のtune種別に応じ初期アドバタイズデータを設定 → BleBroadcast::init()（handle0/1起動）
  → Fluid::init()（IMU初期化）
  → Animations::startMotion()
```

### 9.3 受信 → 表示 → 返信のフルフロー

BLE受信からLED表示・自動返信までは2つのタスクにまたがる。

```
[BTタスク側]
BleBroadcast.cpp:165 handleReceivedPacket()（GAPスキャンコールバック）
  → g_onReceive(payload, len)
  → onMessageReceived() [.ino:149]
      1. TypeByte判定：'I'ならバイナリ192BをRGB配列に復元しJSON化 / 'T'ならJSON文字列そのまま
      2. RX_IGNORE_MS(5000ms)以内の連続受信は完全無視（クールダウン）
      3. 直前と同一内容 かつ RX_SAME_REJECT_MS(5分)以内なら完全リジェクト（表示/保存/返信なし）
      4. Inbox::append(json) で履歴保存
      5. newMessageFlag = true, g_needReply = true をセットするのみ（表示処理はしない）

[メインタスク側] loop() L386-407
  newMessageFlag == true
    → Fluid::syncFromInbox()  … 待機画面の粒を増やす
    → Content::parse(receivedRawJson) 成功 → showContentAfterAnim = true
    → g_needReply == true → 自分のtuneを即座に返信送信（BleBroadcast::send / sendImageRgb）
      ※ 表示完了を待たず即返信するため、相手側もほぼ同時にtuneを受け取れる
    → Animations::startRipple()（遷移演出、ブロッキング実行）

  次ループ: showContentAfterAnim == true
    → showContent(parsedContent, true)
    → Display::showImage(..., RX_DISPLAY_HOLD_MS, animated=true)
       or Display::showText(..., loop=false)
    → 1ピクセルずつ描画するアニメーション表示を開始
```

**要件との対応**: 受信＝即返信により「タッチ（すれ違い）で相互にtuneが交換される」という製品要件を実現している。

### 9.4 待機状態への復帰

```
Display::tick() [Display.cpp:109-151]（毎ループ呼び出し）
  → ピクセルアニメ完了後 g_untilMs = millis() + durationMs をセット
  → millis() >= g_untilMs で clear()（表示終了）

loop() L427: if (!Display::isShowing()) Fluid::tick();
  → 表示終了を isShowing() で検知した瞬間、待機（流体）表示に自動復帰
```

### 9.5 スマホ（GATT）からの書き込みフロー（BleGatt.cpp）

```
スマホが GATT_RX_UUID に書き込み（'{'始まり〜'}'終わりで1件と判定）
  → WriteCallback::onWrite() → g_pendingReady = true
  → BleGatt::tick()（loop()内で毎回呼ばれる）
      flag == "get_settings" → 現在の設定JSONをNotify送信
      flag == "settings"     → setting.json に保存 → Settings::loadFromStorage(true)
                                → Animations::startRipple() → 新色を反映
      それ以外（tune本体）    → Content::saveOwn() でSD+メモリに保存
                                → PHONE_WRITE_PREVIEW_MS(3000ms)だけプレビュー表示
                                → BleBroadcast::send/sendImageRgb で handle0 broadcastへ即反映
                                  （以後、周囲のturnieへ配信されるtuneが更新される）
```

未接続時は `GATT_ADV_WATCHDOG_MS`(3000ms)ごとに handle 1 広告を再アサートし、切断後の広告再開失敗による「発見不能化」を自己修復する（BleGatt.cpp L150-161）。

### 9.6 ボタン操作の分岐（loop() L257-352）

| ボタン | 操作 | 動作 |
|---|---|---|
| BOOT | 200ms〜1.5秒未満押下 | OTAモード（`startDebugMode()`）へ移行 |
| BOOT | 1.5秒以上長押し | 紫点灯2秒→全データリセット（JSON削除、設定初期化、Inbox初期化） |
| USER | 長押し800ms | 閲覧モードのトグル（開始/終了、DiagonalWave演出） |
| USER | 短押し（閲覧モード中） | 履歴を1件戻す（`browseNext()`） |
| USER | 短押し（通常時） | ピクチャ確認モードのトグル（`togglePreview()`） |

### 9.7 既知の実装上の注意点（論文記述時に注意）

- `RX_DISPLAY_GUARD_MS`（Config.h:27, 4500ms）は**コード上どこからも参照されていない未使用の定数**。「受信アニメ中のガード」という設計意図はコメントのみに存在し、実装には反映されていない。
- `Animations::isRunning()` は**常に `false` を返す固定実装**（Animations.cpp:29）。そのため `loop()` の `if (Animations::isRunning())` 分岐は事実上到達せず、Ripple / DiagonalWave は非同期ステートマシンではなく `delay()` を用いた同期・ブロッキング処理として動作する。「アニメーション管理システムによる非同期演出」と書くと実装と乖離するため注意。
- 受信直後の返信 (`g_needReply`) は表示完了を待たずに送信される。つまり「表示→返信」ではなく「即返信→（並行して）表示」という順序である。
