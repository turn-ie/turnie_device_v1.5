#pragma once
#include <Arduino.h>

namespace Settings {

    // setting.json を読み込み、グローバル設定値へ反映する。
    // updateGattName=true の場合は BLE GATT 広告名も更新する。
    bool loadFromStorage(bool updateGattName = false);

}
