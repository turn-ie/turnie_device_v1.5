# turnie デバイス 低電力化プラン

## 背景（なぜ）
電池駆動の turnie（ESP32-S3 + 8x8 WS2812 マトリクス + BLE5）をできるだけ長く動かしたい。
現状の消費電流は次の3点が支配的で、いずれも「常時フル稼働」になっている。

1. **LED マトリクス**: アイドル時も自分のコンテンツを常時表示している。
   - [turnie_device_v1.5.ino](turnie_device_v1.5.ino) の loop 末尾 `if (!Display::isShowing()) showContent(Content::own());`
   - 自分の画像は `ULONG_MAX`（無期限点灯）、自分のテキストは無限スクロール。64個のWS2812が常に描画され続ける＝最大の消費源。
   - 明るさ: [Config.h:19](Config.h#L19) `LED_BRIGHTNESS=20` / [Config.h:81](Config.h#L81) `MOTION_BRIGHTNESS`（既定50）。
2. **BLE スキャン**: [BleBroadcast.cpp:302](BleBroadcast.cpp#L302) `esp_ble_gap_start_ext_scan(0, 0)` ＝ **連続スキャン（停止しない）**。
   - [Config.h:53-54](Config.h#L53-L54) interval 50ms / window 30ms ＝ 無線オン率 **約60%を常時**。
3. **BLE アドバタイズ**: [Config.h:49-50](Config.h#L49-L50) 120ms間隔・TXパワー **+8dBm（ほぼ最大）** を常時。

加えて CPU は [Config.h:25](Config.h#L25) `LOOP_TICK_MS=16` の `delay()` ループのみで、ライトスリープ/省電力管理が無効（S3 240MHz常時）。

**方針**: 「バランス（多少の妥協OK）」。許容トレードオフは
**表示の減光・アイドル消灯／すれ違い検出の頻度ダウン／電波到達距離ダウン**。
**ディープスリープ等でBLE交換が止まるスタンバイは対象外**。
→ 体験を大きく損なわずに、効果の大きいレバーを組み合わせて駆動時間を延ばす。

> 注: 各効果はあくまで概算で、実機計測で確認する前提。

---

## 推奨する施策（効果順）

### 1. LED マトリクス: アイドル消灯 ＋ 減光（最大のレバー）
- **アイドル消灯**: 受信・ボタン操作が一定時間なければマトリクスを消灯し、自分のコンテンツの再表示を止める。USERボタン or BLE受信で復帰。
  - 再利用できる既存部品: `Display::clear()` / `Display::isShowing()`（[Display.cpp](Display.cpp)）、受信復帰は既存の `newMessageFlag` 経路、ボタン復帰は既存の USER ボタンハンドリング（[turnie_device_v1.5.ino](turnie_device_v1.5.ino)）。
  - 新しい定数（例 `IDLE_DISPLAY_OFF_MS`）を [Config.h](Config.h) に追加し、loop の「アイドル時 own 表示」分岐をタイムアウト判定に変える。
- **減光**: `LED_BRIGHTNESS` / `MOTION_BRIGHTNESS` の既定値を下げる。WS2812 の電流は明るさ・点灯ピクセル数にほぼ比例。
- **ハード補足（コード不要）**: WS2812 は黒表示でも 1個あたり ~0.5–1mA の待機電流（64個で ~30–64mA）。完全に切るにはLED電源(5V)ラインにハイサイドの P-MOSFET を入れGPIOでゲートする改造が必要。「真の表示OFF」をやるならこのHW追加が有効。

### 2. BLE スキャン: 連続 → 間欠（大きいレバー）
- **方法A（duty調整）**: [Config.h:53-54](Config.h#L53-L54) の interval を広げる/ window を狭める。
  例: interval 100–160ms・window 30ms → 無線オン率 約19–30%（現状60%から低下）。
- **方法B（周期スキャン）**: [BleBroadcast.cpp:302](BleBroadcast.cpp#L302) を `esp_ble_gap_start_ext_scan(duration, period)` の周期動作にする（例: 1秒スキャン/3秒周期）。
  - `esp_ble_gap_start_ext_scan` は duration と period を取る（GAP API どおり）。OFF区間の分だけスキャン無線エネルギーを削減。
  - トレードオフ: すれ違い検出のレイテンシが OFF区間分だけ増える（許容済み）。
- バランス重視なら A を中心に、必要なら B を軽めに併用。

### 3. BLE アドバタイズ: TXパワー減 ＋（任意で）間隔拡大
- **TXパワー**: [Config.h:50](Config.h#L50) `BLE_EXT_ADV_TX_POWER` を +8dBm → 0dBm 程度へ。1バースト当たりの無線エネルギーが減り、到達距離も短くなる（許容済み）。
- **間隔（任意）**: [Config.h:49](Config.h#L49) `BLE_EXT_ADV_INTERVAL` を 120ms → 200ms 程度へ。アドバタイズのバースト回数が減る。検出レイテンシが少し増えるのでバランスの範囲で。
- channel_map は3chのまま（減らすと信頼性低下のため触らない）。

### 4. CPU / システム省電力（UX無害・定常で効く）
- **自動ライトスリープ + BLE モデムスリープ**: ESP-IDF の電力管理を有効化（`CONFIG_PM_ENABLE` + `CONFIG_FREERTOS_USE_TICKLESS_IDLE` + 起動時に `esp_pm_configure(min/max freq, light_sleep_enable=true)`）。BLE接続は維持したまま、イベント間でCPUがライトスリープする。スタンバイではないので対象外要件に抵触しない。
  - PlatformIO では `build_flags` / カスタム `sdkconfig` 変更が必要（Arduino の1行では完結しない）。
- **最大CPU周波数の引き下げ**: 80–160MHz へ（`esp_pm` か `setCpuFrequencyMhz`）。効果は中程度。BLEは≥80MHz必要。
- **USB CDC**: [platformio.ini](platformio.ini) `ARDUINO_USB_CDC_ON_BOOT=1` で USB が常時有効。デバッグ不要時に無効化すると微小に減るが、シリアルログを失う（任意）。

### 5. ハードウェアのみ（コード変更ゼロ）
- WS2812 5Vラインのハイサイド MOSFET 電源ゲート（上記1の待機電流カット）。
- 容量の大きい/良質な電池、効率の良いLDOまたはDC-DC(buck)への変更。

---

## 推奨バランス構成（まとめ）
1（アイドル消灯＋減光）＋ 2（スキャンを duty ~25–30% か 1s/3s 周期）＋ 3（TX 0dBm・adv ~200ms）＋ 4（自動ライトスリープ有効化）。
**ディープスリープ/スタンバイは要件どおり不採用。**
最も効くのは通常 1 の「アイドル消灯」（常時描画 → 受信待ちの無線+CPUフロアのみへ）。2・4 が定常フロアをさらに削る。

---

## 検証方法（効果と健全性の確認）
- **電流計測**: USB電力計 or 電池ライン直列の電流計で、施策前後を状態別に比較。
  - アイドル（相手なし）／アドバタイズ+スキャン中／交換中／表示ON vs 表示OFF。
  - mA を比較し、電池容量に対する mAh/日 を見積もって駆動時間を試算。
- **機能確認**:
  - すれ違い検出が許容レイテンシ内で動くこと（スキャン間欠化後）。
  - 到達距離が実用範囲であること（TXパワー低下後）。
  - 自動ライトスリープ有効時に BLE 交換・GATT（スマホ連携）が正常動作すること。
  - アイドル消灯からボタン/受信で正しく復帰すること。
