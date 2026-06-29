#include "BleBroadcast.h"
#include "BleStack.h"
#include "Config.h"

#include "esp_gap_ble_api.h"
#include <BLEDevice.h>

// ============================================================
// 内部状態
// ============================================================
static BleBroadcast::ReceiveHandler g_onReceive = nullptr;
static String g_initialJson = "";
// 初期データが画像の場合のバイナリキャッシュ
static uint8_t g_initialRgb[192];
static bool g_initialIsImage = false;

// handle 0/1 の起動完了フラグ
// どちらの EXT_ADV_SET_PARAMS / DATA_SET イベントかを判別するために使う
// （IDF のこのバージョンでは instance フィールドが存在しないため）
static bool g_handle0Ready = false;
static bool g_handle1Ready = false;

// ============================================================
// handle 0 用バッファ（デバイス間 JSON、non-connectable）
// ============================================================
// テキストは ~200 バイト、画像は 'I'(1) + RGB(192) = 193 バイト
// 4 ヘッダ(0xFF + CompID + TypeByte) + 196 データ = 200 で余裕あり
static const int MAX_PAYLOAD = 200;
static uint8_t g_advBuf[MAX_PAYLOAD + 8];
static int g_advBufLen = 0;

// ============================================================
// handle 1 用バッファ（スマホ向け GATT、legacy 互換 connectable）
//   Advertising Data: Flags + 128bit UUID
//   Scan Response:    デバイス名
// ============================================================
static uint8_t g_gattAdvBuf[32];
static int g_gattAdvBufLen = 0;

static uint8_t g_gattScanRspBuf[32];
static int g_gattScanRspBufLen = 0;

// ============================================================
// パケット構造
//   [Length][0xFF][CompID_L][CompID_H][TypeByte][Data...]
//
//   TypeByte:
//     'T' (0x54) = テキスト JSON
//     'I' (0x49) = バイナリ画像（192 バイト RGB データ）
//
//   画像を JSON で送ると ~800 バイトになり MAX_PAYLOAD を超えるため
//   バイナリ形式を使う。バイナリなら 1 + 192 = 193 バイトで収まる。
// ============================================================

// テキスト JSON パケットを組み立てる
static bool buildTextPacket(const String &json) {
  int jsonLen = json.length();
  // 4 = 0xFF + CompID(2) + TypeByte(1)
  if (4 + jsonLen > MAX_PAYLOAD) {
    Serial.println("  [BleBroadcast] text too long (" + String(jsonLen) +
                   " bytes)");
    return false;
  }
  int index = 0;
  g_advBuf[index++] = (uint8_t)(4 + jsonLen); // Length
  g_advBuf[index++] = 0xFF;
  g_advBuf[index++] = (uint8_t)(BLE_COMPANY_ID & 0xFF);
  g_advBuf[index++] = (uint8_t)((BLE_COMPANY_ID >> 8) & 0xFF);
  g_advBuf[index++] = 'T'; // TypeByte: Text
  for (int i = 0; i < jsonLen; i++) {
    g_advBuf[index++] = (uint8_t)json[i];
  }
  g_advBufLen = index;
  return true;
}

// バイナリ画像パケットを組み立てる（192 バイト RGB）
static bool buildImagePacket(const uint8_t *rgb, int rgbLen) {
  // 4 = 0xFF + CompID(2) + TypeByte(1)
  if (4 + rgbLen > MAX_PAYLOAD) {
    Serial.println("  [BleBroadcast] image too large (" + String(rgbLen) +
                   " bytes)");
    return false;
  }
  int index = 0;
  g_advBuf[index++] = (uint8_t)(4 + rgbLen); // Length
  g_advBuf[index++] = 0xFF;
  g_advBuf[index++] = (uint8_t)(BLE_COMPANY_ID & 0xFF);
  g_advBuf[index++] = (uint8_t)((BLE_COMPANY_ID >> 8) & 0xFF);
  g_advBuf[index++] = 'I'; // TypeByte: Image
  memcpy(g_advBuf + index, rgb, rgbLen);
  index += rgbLen;
  g_advBufLen = index;
  return true;
}

// 後方互換用（内部では buildTextPacket を呼ぶ）
static bool buildAdvPacket(const String &json) { return buildTextPacket(json); }

// スキャンレスポンスデータを組み立てる（デバイス名）
static void buildGattScanRsp() {
  int idx = 0;
  const char *name = DEVICE_NAME[0] ? DEVICE_NAME : BLE_DEVICE_NAME;
  int nameLen = strlen(name);
  g_gattScanRspBuf[idx++] = (uint8_t)(nameLen + 1);
  g_gattScanRspBuf[idx++] = 0x09; // Type: Complete Local Name
  memcpy(g_gattScanRspBuf + idx, name, nameLen);
  idx += nameLen;
  g_gattScanRspBufLen = idx;
}

// ============================================================
// handle 1 用アドバタイズデータを組み立てる
//   Advertising Data: Flags + Service UUID（21 バイト）
//   Scan Response:    デバイス名（別バッファ）
// ============================================================
static void buildGattAdvPacket() {
  int idx = 0;

  // AD: Flags（LE General Discoverable + BR/EDR Not Supported）
  g_gattAdvBuf[idx++] = 0x02;
  g_gattAdvBuf[idx++] = 0x01;
  g_gattAdvBuf[idx++] = 0x06;

  // AD: Complete List of 128-bit UUIDs
  // GATT_SVC_UUID "12345678-1234-1234-1234-1234567890ab" → リトルエンディアン
  g_gattAdvBuf[idx++] = 0x11;
  g_gattAdvBuf[idx++] = 0x07;
  const uint8_t uuid128[] = {0xab, 0x90, 0x78, 0x56, 0x34, 0x12, 0x34, 0x12,
                             0x34, 0x12, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12};
  memcpy(g_gattAdvBuf + idx, uuid128, 16);
  idx += 16;
  g_gattAdvBufLen = idx; // 21 bytes

  // デバイス名はスキャンレスポンスに格納
  buildGattScanRsp();
}

// ============================================================
// handle 1（GATT 用、legacy 互換 connectable）のパラメータ設定を開始する
//   LEGACY フラグで primary channel に ADV_IND として送出される。
//   handle 0 と同じ Extended Advertising API を使うため HCI 競合しない。
// ============================================================
static void startGattAdvSetup() {
  static esp_ble_gap_ext_adv_params_t gattParams;
  memset(&gattParams, 0, sizeof(gattParams));
  gattParams.type = ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY |
                    ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE |
                    ESP_BLE_GAP_SET_EXT_ADV_PROP_SCANNABLE;
  gattParams.interval_min = 0x00A0; // 100 ms
  gattParams.interval_max = 0x00A0;
  gattParams.channel_map = ADV_CHNL_ALL;
  gattParams.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
  gattParams.filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
  gattParams.tx_power = BLE_EXT_ADV_TX_POWER;
  gattParams.primary_phy = ESP_BLE_GAP_PHY_1M;
  gattParams.max_skip = 0;
  gattParams.secondary_phy = ESP_BLE_GAP_PHY_1M;
  gattParams.sid = 1;
  gattParams.scan_req_notif = false;
  esp_ble_gap_ext_adv_set_params(BLE_GATT_ADV_HANDLE, &gattParams);
}

// ============================================================
// 受信パケットを解析して JSON ペイロードを取り出す
// ============================================================
static void handleReceivedPacket(const esp_ble_gap_ext_adv_reprot_t &report) {
  if (g_onReceive == nullptr)
    return;
  if (report.rssi < BLE_RSSI_MIN)
    return;
  if (memcmp(report.addr, BleStack::myMac(), 6) == 0)
    return;

  const uint8_t *ptr = report.adv_data;
  int remaining = (int)report.adv_data_len;

  while (remaining > 1) {
    int adLen = (int)ptr[0];
    uint8_t adType = ptr[1];
    if (adLen == 0)
      break;
    if (remaining < adLen + 1)
      break;
    // TypeByte を含むため adLen >= 4（CompID 2 + TypeByte 1 + data 1 以上）
    if (adType == 0xFF && adLen >= 4) {
      uint16_t companyId = ptr[2] | ((uint16_t)ptr[3] << 8);
      if (companyId == BLE_COMPANY_ID) {
        // ptr[4] = TypeByte ('T' or 'I'), ptr[5..] = data
        const uint8_t *payload = ptr + 4; // TypeByte から始まる
        int payloadLen = adLen - 3;       // TypeByte + data のバイト数
        g_onReceive(payload, (size_t)payloadLen);
        return;
      }
    }
    ptr += adLen + 1;
    remaining -= adLen + 1;
  }
}

// ============================================================
// GAP コールバック
//   handle 0（non-connectable）→ handle 1（legacy connectable）の順に
//   非同期ステートマシンで初期化する。
//
//   どちらの handle のイベントかは g_handle0Ready / g_handle1Ready で判別する。
//   （IDF のこのバージョンでは EXT_ADV_SET_PARAMS_COMPLETE_EVT 等に
//    instance フィールドが存在しないため）
// ============================================================
static void customGapCallback(esp_gap_ble_cb_event_t event,
                              esp_ble_gap_cb_param_t *param) {

  // --- Extended Advertising パラメータ設定完了 ---
  if (event == ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT) {
    if (param->ext_adv_set_params.status != ESP_BT_STATUS_SUCCESS) {
      Serial.println("  [BleBroadcast] adv params failed, status=" +
                     String(param->ext_adv_set_params.status));
      return;
    }
    if (!g_handle0Ready) {
      Serial.println("  [BleBroadcast] adv params set (handle 0)");
      esp_ble_gap_config_ext_adv_data_raw(BLE_EXT_ADV_HANDLE, g_advBufLen,
                                          g_advBuf);
    } else {
      Serial.println("  [BleBroadcast] GATT adv params set (handle 1)");
      esp_ble_gap_config_ext_adv_data_raw(BLE_GATT_ADV_HANDLE, g_gattAdvBufLen,
                                          g_gattAdvBuf);
    }
    return;
  }

  // --- アドバタイズデータ設定完了 ---
  if (event == ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT) {
    if (param->ext_adv_data_set.status != ESP_BT_STATUS_SUCCESS) {
      Serial.println("  [BleBroadcast] adv data failed, status=" +
                     String(param->ext_adv_data_set.status));
      return;
    }
    if (!g_handle0Ready) {
      // handle 0 のデータ設定完了 → handle 0 を起動する
      Serial.println("  [BleBroadcast] adv data set (handle 0), starting...");
      esp_ble_gap_ext_adv_t advCfg;
      advCfg.instance = BLE_EXT_ADV_HANDLE;
      advCfg.duration = 0;
      advCfg.max_events = 0;
      esp_ble_gap_ext_adv_start(1, &advCfg);
    } else if (!g_handle1Ready) {
      // handle 1 のアドバタイズデータ設定完了 → スキャンレスポンスを設定する
      Serial.println(
          "  [BleBroadcast] GATT adv data set (handle 1), setting scan rsp...");
      esp_ble_gap_config_ext_scan_rsp_data_raw(BLE_GATT_ADV_HANDLE,
                                               g_gattScanRspBufLen,
                                               g_gattScanRspBuf);
    }
    // g_handle0Ready && g_handle1Ready の場合は send()
    // によるデータ更新なので何もしない
    return;
  }

  // --- スキャンレスポンスデータ設定完了 ---
  if (event == ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT) {
    if (!g_handle1Ready) {
      Serial.println(
          "  [BleBroadcast] GATT scan rsp set (handle 1), starting...");
      esp_ble_gap_ext_adv_t advCfg;
      advCfg.instance = BLE_GATT_ADV_HANDLE;
      advCfg.duration = 0;
      advCfg.max_events = 0;
      esp_ble_gap_ext_adv_start(1, &advCfg);
    }
    return;
  }

  // --- Extended Advertising 開始完了 ---
  if (event == ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT) {
    if (param->ext_adv_start.status != ESP_BT_STATUS_SUCCESS) {
      Serial.println("  [BleBroadcast] adv start failed, status=" +
                     String(param->ext_adv_start.status));
      return;
    }
    if (!g_handle0Ready) {
      g_handle0Ready = true;
      Serial.println(
          "  [BleBroadcast] adv started (handle 0). setting up GATT adv...");
      startGattAdvSetup();
    } else if (!g_handle1Ready) {
      g_handle1Ready = true;
      Serial.println("  [BleBroadcast] GATT advertising started (handle 1). "
                     "BLE fully ready!");
    }
    return;
  }

  // --- Extended Scan パラメータ設定完了 ---
  if (event == ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT) {
    if (param->set_ext_scan_params.status != ESP_BT_STATUS_SUCCESS) {
      Serial.println("  [BleBroadcast] scan params failed, status=" +
                     String(param->set_ext_scan_params.status));
      return;
    }
    Serial.println("  [BleBroadcast] scan params set. starting scan...");
    esp_ble_gap_start_ext_scan(0, 0);
    return;
  }

  // --- Extended Scan 開始完了 ---
  if (event == ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT) {
    if (param->ext_scan_start.status != ESP_BT_STATUS_SUCCESS) {
      Serial.println("  [BleBroadcast] scan start failed, status=" +
                     String(param->ext_scan_start.status));
      return;
    }
    Serial.println("  [BleBroadcast] scan started.");
    return;
  }

  // --- 他デバイスの広告を受信 ---
  if (event == ESP_GAP_BLE_EXT_ADV_REPORT_EVT) {
    handleReceivedPacket(param->ext_adv_report.params);
    return;
  }
}

// ============================================================
// Public API
// ============================================================
namespace BleBroadcast {

bool init() {
  buildGattAdvPacket();

  if (g_initialIsImage) {
    // 画像はバイナリパケット（193バイト）で初期化
    if (!buildImagePacket(g_initialRgb, 192)) {
      Serial.println("  [BleBroadcast] initial image packet failed");
    }
  } else if (!g_initialJson.isEmpty()) {
    if (!buildTextPacket(g_initialJson)) {
      // テキストでも長すぎる場合はデバイス名のみにフォールバック
      buildTextPacket("{\"name\":\"" +
                     String(DEVICE_NAME[0] ? DEVICE_NAME : BLE_DEVICE_NAME) +
                     "\"}");
    }
  } else {
    buildTextPacket("{\"name\":\"" +
                   String(DEVICE_NAME[0] ? DEVICE_NAME : BLE_DEVICE_NAME) +
                   "\"}");
  }

  // Arduino BLE の GAP callback を差し替える（GATTS は別 callback
  // なので影響なし）
  esp_ble_gap_register_callback(customGapCallback);

  // handle 0（non-connectable）パラメータ設定
  static esp_ble_gap_ext_adv_params_t advParams;
  memset(&advParams, 0, sizeof(advParams));
  advParams.type = ESP_BLE_GAP_SET_EXT_ADV_PROP_NONCONN_NONSCANNABLE_UNDIRECTED;
  advParams.interval_min = BLE_EXT_ADV_INTERVAL;
  advParams.interval_max = BLE_EXT_ADV_INTERVAL;
  advParams.channel_map = ADV_CHNL_ALL;
  advParams.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
  advParams.filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
  advParams.tx_power = BLE_EXT_ADV_TX_POWER;
  advParams.primary_phy = ESP_BLE_GAP_PHY_1M;
  advParams.max_skip = 0;
  advParams.secondary_phy = ESP_BLE_GAP_PHY_1M;
  advParams.sid = 0;
  advParams.scan_req_notif = false;

  esp_err_t result =
      esp_ble_gap_ext_adv_set_params(BLE_EXT_ADV_HANDLE, &advParams);
  if (result != ESP_OK) {
    Serial.println("  [BleBroadcast] adv init failed: " +
                   String(esp_err_to_name(result)));
    return false;
  }

  // スキャンパラメータ設定（adv と並行して非同期に進む）
  static esp_ble_ext_scan_params_t scanParams;
  memset(&scanParams, 0, sizeof(scanParams));
  scanParams.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
  scanParams.filter_policy = BLE_SCAN_FILTER_ALLOW_ALL;
  scanParams.scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE;
  scanParams.cfg_mask = ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK;
  scanParams.uncoded_cfg.scan_type = BLE_SCAN_TYPE_PASSIVE;
  scanParams.uncoded_cfg.scan_interval = BLE_EXT_SCAN_INTERVAL;
  scanParams.uncoded_cfg.scan_window = BLE_EXT_SCAN_WINDOW;

  result = esp_ble_gap_set_ext_scan_params(&scanParams);
  if (result != ESP_OK) {
    Serial.println("  [BleBroadcast] scan init failed: " +
                   String(esp_err_to_name(result)));
    return false;
  }

  Serial.println("  [BleBroadcast] init started (async state machine running)");
  return true;
}

void setOnReceive(ReceiveHandler handler) { g_onReceive = handler; }

void setInitialData(const String &json) { g_initialJson = json; }

// 画像コンテンツを初期データとして設定する（バイナリ形式で保持）
void setInitialImageRgb(const uint8_t *rgb, int rgbLen) {
  if (rgbLen != 192) return;
  memcpy(g_initialRgb, rgb, 192);
  g_initialIsImage = true;
  g_initialJson = ""; // テキスト側は無効化
}

void send(const String &json) {
  if (!buildTextPacket(json))
    return;
  if (g_handle0Ready) {
    esp_ble_gap_config_ext_adv_data_raw(BLE_EXT_ADV_HANDLE, g_advBufLen,
                                        g_advBuf);
    Serial.println("  [TX] text: " + json);
  }
}

void sendImageRgb(const uint8_t *rgb, int rgbLen) {
  if (!buildImagePacket(rgb, rgbLen))
    return;
  if (g_handle0Ready) {
    esp_ble_gap_config_ext_adv_data_raw(BLE_EXT_ADV_HANDLE, g_advBufLen,
                                        g_advBuf);
    Serial.printf("  [TX] image (%d bytes binary)\n", rgbLen);
  }
}

void restartGattAdv() {
  // handle 1 の connectable 広告を（再）開始する。
  // 切断時の再開と、未接続中のウォッチドッグ再アサートの両方から呼ばれる。
  // 既に広告中なら no-op 相当。失敗時のみログを出す（成功時は無音 = ログ汚染防止）。
  esp_ble_gap_ext_adv_t advCfg;
  advCfg.instance = BLE_GATT_ADV_HANDLE;
  advCfg.duration = 0;
  advCfg.max_events = 0;
  esp_err_t err = esp_ble_gap_ext_adv_start(1, &advCfg);
  if (err != ESP_OK) {
    Serial.println("  [BleBroadcast] GATT adv restart failed: " +
                   String(esp_err_to_name(err)));
  }
}

void updateGattName() {
  // DEVICE_NAME が変更されたとき、以下の2つを更新する：
  //   1. GAP Device Name キャラクタリスティック（0x2A00）
  //   2. スキャンレスポンス内の Complete Local Name
  const char *newName = DEVICE_NAME[0] ? DEVICE_NAME : BLE_DEVICE_NAME;
  esp_ble_gap_set_device_name(newName);

  buildGattScanRsp();
  esp_ble_gap_config_ext_scan_rsp_data_raw(BLE_GATT_ADV_HANDLE,
                                           g_gattScanRspBufLen,
                                           g_gattScanRspBuf);
  Serial.printf("  [BleBroadcast] GATT name updated to: %s\n", newName);
}

} // namespace BleBroadcast
