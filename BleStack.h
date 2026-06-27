#pragma once
#include <Arduino.h>

// =============================================================
// BleStack
//   BLE スタックの初期化と、自分の MAC アドレスの提供。
//   Arduino BLE ライブラリ（BLEDevice::init）を呼んで
//   スタック全体を起動する。
// =============================================================
namespace BleStack {

    // BLE スタックを初期化する（BLEDevice::init + MAC 取得）
    bool init();

    // 自分の Bluetooth MAC アドレス（init() 後に有効）
    const uint8_t* myMac();

}
