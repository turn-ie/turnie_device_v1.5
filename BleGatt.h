#pragma once
#include <Arduino.h>

// =============================================================
// BleGatt
//   GATT サーバー。スマートフォンからの JSON 受信を担当する。
//   Arduino BLE ライブラリ（BLEDevice）を使って実装する。
//
//   スマホは BLE で接続し、RX キャラクタリスティックに JSON を書き込む。
//   受信が完了したら "saved" を TX キャラクタリスティックで Notify する。
//   実際の保存・表示処理は割り込みの外で tick() が行う。
// =============================================================
namespace BleGatt {

    // GATT サービスとキャラクタリスティックを登録する
    // ※ BleStack::init()（= BLEDevice::init()）の後に呼ぶこと
    bool init();

    // loop() から毎回呼ぶ（受信 JSON の保存・表示処理を行う）
    void tick();

}
