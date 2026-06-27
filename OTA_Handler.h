#pragma once
#include <Arduino.h>

// =============================================================
// OTA_Handler
//   ユーザーボタン長押し（3秒）でデバッグモードに移行する。
//   WiFi 接続後に ArduinoOTA（書き込み）と
//   Telnet コンソール（ポート 23）を提供する。
//
//   WiFi SSID / PASS は Config.h の OTA_WIFI_SSID / OTA_WIFI_PASS で指定。
// =============================================================

// デバッグモード中か確認する
bool isDebugMode();

// デバッグモードを開始する（WiFi 接続 + OTA + Telnet サーバー起動）
void startDebugMode();

// デバッグモードの loop 処理（OTA ハンドル + Telnet コマンド処理）
// loop() の先頭から毎回呼ぶ
void handleDebugMode();
