#include "BleStack.h"
#include "Config.h"

#include <BLEDevice.h>
#include "esp_mac.h"

// ============================================================
// 内部状態
// ============================================================
static uint8_t g_myMac[6] = {0};

// ============================================================
// Public API
// ============================================================
namespace BleStack {

bool init() {
    // Arduino BLE ライブラリがスタック初期化・デバイス名設定を全部やってくれる
    BLEDevice::init(DEVICE_NAME[0] ? DEVICE_NAME : BLE_DEVICE_NAME);

    // 自分の Bluetooth MAC アドレスを取得して保存
    esp_read_mac(g_myMac, ESP_MAC_BT);

    Serial.printf("  [BleStack] ready. MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
        g_myMac[0], g_myMac[1], g_myMac[2],
        g_myMac[3], g_myMac[4], g_myMac[5]);
    return true;
}

const uint8_t* myMac() {
    return g_myMac;
}

} // namespace BleStack
