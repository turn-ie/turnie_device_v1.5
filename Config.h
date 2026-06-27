#pragma once
#include <Arduino.h>

// ============================================================
// Hardware Pins
// ============================================================
constexpr int PIN_MATRIX = 14;   // NeoMatrix データ
constexpr int PIN_BTN_USER =  39; // ユーザーボタン（INPUT_PULLUP、LOW=押下）
constexpr int PIN_BTN_BOOT = 0;  // BOOT ボタン（GPIO0、押下で OTA モードへ）

// ============================================================
// Display (8x8 LED Matrix)
// ============================================================
constexpr int MATRIX_WIDTH = 8;
constexpr int MATRIX_HEIGHT = 8;
constexpr int MATRIX_PIXEL_COUNT = MATRIX_WIDTH * MATRIX_HEIGHT;
constexpr int MATRIX_RGB_BYTES = MATRIX_PIXEL_COUNT * 3;

// 明るさは全モード共通で MOTION_BRIGHTNESS（スマホ設定値）を使う。
constexpr uint16_t TEXT_SCROLL_DELAY_MS = 70;

// ============================================================
// Timing
// ============================================================
constexpr unsigned long LOOP_TICK_MS = 16;          // ~60 fps
constexpr unsigned long RX_DISPLAY_HOLD_MS = 4500;  // 受信表示の保持時間
constexpr unsigned long RX_DISPLAY_GUARD_MS = 4500; // 受信アニメ中のガード時間
constexpr unsigned long RX_IGNORE_MS = 5000;        // 一度受信したら、この時間は内容に関わらず後続の受信を全て無視する
constexpr unsigned long RX_SAME_REJECT_MS = 180000; // 直前に受け付けたのと同じ内容がこの時間（5分）以内に届いた場合、完全にリジェクトする（表示も保存も返信もしない）。時間を超えれば同じ内容でも改めて受け付ける。

// 受信履歴の閲覧モード（USER ボタン操作）
constexpr unsigned long BROWSE_LONG_PRESS_MS = 800;    // 長押し判定（閲覧モード開始/終了）
constexpr unsigned long BUTTON_DEBOUNCE_MS = 50;       // 短押しのチャタリング除去
constexpr unsigned long BROWSE_TIMEOUT_MS = 30000;     // 無操作でこの時間経過したら通常へ戻る

// ============================================================
// Storage (内部フラッシュ - LittleFS)
// ============================================================
constexpr const char *SD_OWN_JSON = "/data.json";
constexpr const char *SD_INBOX_LOG = "/inbox_log.txt";

// ============================================================
// BLE Identity
// ============================================================
constexpr const char *BLE_DEVICE_NAME = "turnie"; // デバイスごとに変更
constexpr uint16_t BLE_COMPANY_ID = 0xFFFF;       // turnie 間の識別子

// ============================================================
// BLE5 Extended Advertising
// ============================================================
constexpr uint8_t BLE_EXT_ADV_HANDLE =
    0; // デバイス間ブロードキャスト（non-connectable）
constexpr uint8_t BLE_GATT_ADV_HANDLE =
    1; // スマホ向け GATT 広告（connectable）
constexpr uint32_t BLE_EXT_ADV_INTERVAL = 0x00C0; // 120 ms (n * 0.625 ms)
constexpr int8_t BLE_EXT_ADV_TX_POWER = 8;        // dBm

// BLE5 Extended Scan
constexpr uint16_t BLE_EXT_SCAN_INTERVAL = 0x0050; // 50 ms
constexpr uint16_t BLE_EXT_SCAN_WINDOW = 0x0030;   // 30 ms

// ============================================================
// BLE GATT (smartphone interface)
// ============================================================
constexpr uint16_t GATT_APP_ID = 0;
constexpr const char *GATT_SVC_UUID = "12345678-1234-1234-1234-1234567890ab";
constexpr const char *GATT_RX_UUID = "abcd1234-5678-90ab-cdef-1234567890ab";
constexpr const char *GATT_TX_UUID = "abcd1234-5678-90ab-cdef-1234567890ac";

// ============================================================
// Inbox (received messages, RAM ring buffer)
// ============================================================
constexpr size_t INBOX_CAPACITY = 30;

// ============================================================
// BLE Settings
// ============================================================
// RSSI の閾値（これより弱い信号は無視する）
// 密着状態で -30 ~ -40, 1m離れて -60 程度
// -128 = 実質すべての受信を許可（int8_t の最小値）
constexpr int8_t BLE_RSSI_MIN = -128;

// ============================================================
// Animation Hue & Settings (per-device config)
// ============================================================
// 色相 H は 0–360（度）。スマホ(setting.json)から送られてくる値もこの範囲。
//   turnie1=127(赤ピンク) / turnie2=298(青) / turnie3=6(緑) / turnie4=104(黄)
extern uint16_t MOTION_HUE;
extern uint8_t MOTION_BRIGHTNESS;
extern uint8_t MOTION_ANIM;

constexpr int DEVICE_NAME_MAX = 32;
extern char DEVICE_NAME[DEVICE_NAME_MAX];

constexpr const char *SD_SETTING_JSON = "/setting.json";
constexpr const char *SD_INBOX_JSON = "/inbox.json";

// OTA / WiFi の認証情報は Secrets.h に分離（Git には含めない）。
//   初回は Secrets.h.example をコピーして作成する: cp Secrets.h.example Secrets.h
//   定義: OTA_WIFI_SSID/PASS(1..3), OTA_HOSTNAME, OTA_PASSWORD
#include "Secrets.h"
