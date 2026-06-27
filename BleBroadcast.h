#pragma once
#include <Arduino.h>

// =============================================================
// BleBroadcast
//   BLE5 Extended Advertising によるデバイス間ブロードキャスト。
//   送信：JSON をアドバタイズデータとして常時送信する。
//   受信：他デバイスのアドバタイズを受信してコールバックに渡す。
//
//   ※ Phase ⑨ で実装予定
// =============================================================
namespace BleBroadcast {

    using ReceiveHandler = void (*)(const uint8_t* data, size_t len);

    bool init();
    void setOnReceive(ReceiveHandler handler);
    void setInitialData(const String& json);          // テキスト初期データ設定
    void setInitialImageRgb(const uint8_t* rgb, int rgbLen); // 画像初期データ設定
    void send(const String& json);                          // テキスト送信
    void sendImageRgb(const uint8_t* rgb, int rgbLen);     // 画像バイナリ送信
    void restartGattAdv();   // 電話切断後に GATT 広告を再開する
    void updateGattName();   // DEVICE_NAME 変更後に GATT 広告パケットを再構築・再送信する

}
