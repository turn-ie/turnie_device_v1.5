#include "OTA_Handler.h"
#include "Config.h"
#include "Display.h"
#include "Storage.h"

#include <ArduinoOTA.h>
#include <BLEDevice.h>
#include <WiFi.h>

// Telnet サーバー（ポート 23）
static WiFiServer telnetServer(23);
static WiFiClient telnetClient;

static bool s_debugMode = false;
static bool s_otaReady = false;

// ============================================================
// Public API
// ============================================================

bool isDebugMode() { return s_debugMode; }

void startDebugMode() {
  if (s_debugMode)
    return;

      s_debugMode = true;
  Serial.println("\n=== OTA MODE ===");

  Display::fillColor(255, 255, 0); // 黄色: WiFi 接続中

  // BLE を完全停止してから WiFi を起動（共存問題回避）
  Serial.println("[OTA] Stopping BLE...");
  BLEDevice::deinit(true);
  delay(300);

  WiFi.mode(WIFI_STA);

  // 接続できるまで WiFi 1 と WiFi 2 を交互にリトライし続ける
  while (WiFi.status() != WL_CONNECTED) {
    // --- 1つ目のWiFiに接続試行 ---
    WiFi.disconnect(true);
    delay(100);
    WiFi.begin(OTA_WIFI_SSID, OTA_WIFI_PASS);
    Serial.printf("[OTA] Connecting to WiFi 1: %s\n", OTA_WIFI_SSID);
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 100) { // 約5秒待つ
      delay(50);
      retry++;
      if (retry % 20 == 0)
        Serial.print(".");
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED)
      break;

    // --- 2つ目のWiFiに接続試行 ---
    WiFi.disconnect(true);
    delay(100);
    WiFi.begin(OTA_WIFI_SSID2, OTA_WIFI_PASS2);
    Serial.printf("[OTA] Connecting to WiFi 2: %s\n", OTA_WIFI_SSID2);
    retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 100) { // 約5秒待つ
      delay(50);
      retry++;
      if (retry % 20 == 0)
        Serial.print(".");
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED)
      break;

    // --- 3つ目のWiFiに接続試行 ---
    WiFi.disconnect(true);
    delay(100);
    WiFi.begin(OTA_WIFI_SSID3, OTA_WIFI_PASS3);
    Serial.printf("[OTA] Connecting to WiFi 3: %s\n", OTA_WIFI_SSID3);
    retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 100) { // 約5秒待つ
      delay(50);
      retry++;
      if (retry % 20 == 0)
        Serial.print(".");
    }
    Serial.println();
  }

  Display::fillColor(0, 255, 0); // 緑: 接続成功

  // WiFi MAC アドレスから固有ホスト名を生成（BLE MAC を使わない）
  uint8_t mac[6];
  WiFi.macAddress(mac);
  static char hostname[32];
  snprintf(hostname, sizeof(hostname), "turnie-%02x%02x", mac[4], mac[5]);

  // ============================================================
  // ArduinoOTA セットアップ
  //   - パスワード: なし
  //     → Arduino IDE / PlatformIO ともにパスワード不要
  //   - ホスト名: turnie-XXXX.local
  // ============================================================
  ArduinoOTA.setHostname(hostname);
  // ※ ArduinoOTA.setPassword() は呼ばない → パスワードなし

  ArduinoOTA.onStart([]() {
    Serial.println("[OTA] Upload start...");
    Display::clear();
  });
  ArduinoOTA.onEnd([]() { Serial.println("\n[OTA] Done. Rebooting..."); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    uint8_t pct = progress / (total / 100);
    uint8_t bv = (uint8_t)(pct * 255 / 100);
    Display::fillColor(0, 0, bv);
    Serial.printf("[OTA] %u%%\r", pct);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[OTA] Error[%u]\n", error);
    Display::fillColor(255, 0, 0);
  });

  ArduinoOTA.begin();
  s_otaReady = true;

  // Telnet サーバー開始
  telnetServer.begin();
  telnetServer.setNoDelay(true);

  Serial.println("[OTA] Ready!");
  Serial.printf("  IP       : %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("  Hostname : %s.local\n", hostname);
  Serial.println("  Password : (none)");
  Serial.println("  Telnet   : nc <IP> 23");
}

void handleDebugMode() {
  if (!s_debugMode)
    return;

  // Telnet 新規接続受付
  if (telnetServer.hasClient()) {
    if (telnetClient && telnetClient.connected()) {
      telnetClient.stop();
    }
    telnetClient = telnetServer.available();
    telnetClient.println("=== turnie OTA Console ===");
    telnetClient.printf("IP: %s\n", WiFi.localIP().toString().c_str());
    telnetClient.println("Commands: help / status / heap / wifi / dump / reboot");
  }

  // Telnet コマンド処理
  if (telnetClient && telnetClient.connected() && telnetClient.available()) {
    String cmd = telnetClient.readStringUntil('\n');
    cmd.trim();

    if (cmd == "help") {
      telnetClient.println("  help   - このヘルプ");
      telnetClient.println("  status - デバイス状態");
      telnetClient.println("  heap   - ヒープメモリ");
      telnetClient.println("  wifi   - WiFi 情報");
      telnetClient.println("  dump   - 受信履歴(JSON)を出力");
      telnetClient.println("  reboot - 再起動");
    } else if (cmd == "dump") {
      String json = Storage::loadJson(SD_INBOX_JSON);
      if (json.isEmpty()) json = "[]";
      telnetClient.println("===TURNIE_INBOX_BEGIN===");
      telnetClient.println(json);
      telnetClient.println("===TURNIE_INBOX_END===");
    } else if (cmd == "status") {
      unsigned long s = millis() / 1000;
      telnetClient.printf("Uptime : %lu:%02lu\n", s / 60, s % 60);
      telnetClient.printf("Heap   : %u / min %u bytes\n", ESP.getFreeHeap(),
                          ESP.getMinFreeHeap());
      telnetClient.printf("OTA    : %s\n", s_otaReady ? "Ready" : "Not Ready");
    } else if (cmd == "heap") {
      telnetClient.printf("Free Heap : %u bytes\n", ESP.getFreeHeap());
      telnetClient.printf("Min  Heap : %u bytes\n", ESP.getMinFreeHeap());
    } else if (cmd == "wifi") {
      telnetClient.printf("SSID    : %s\n", WiFi.SSID().c_str());
      telnetClient.printf("IP      : %s\n", WiFi.localIP().toString().c_str());
      telnetClient.printf("RSSI    : %d dBm\n", WiFi.RSSI());
      telnetClient.printf("Channel : %d\n", WiFi.channel());
    } else if (cmd == "reboot") {
      telnetClient.println("Rebooting...");
      delay(100);
      ESP.restart();
    } else if (cmd.length() > 0) {
      telnetClient.printf("Unknown: %s\n", cmd.c_str());
    }
  }

  // 定期ステータス（30秒ごと）
  static unsigned long lastStatusMs = 0;
  if (millis() - lastStatusMs > 30000) {
    lastStatusMs = millis();
    if (telnetClient && telnetClient.connected()) {
      unsigned long s = millis() / 1000;
      telnetClient.println("\n--- STATUS ---");
      telnetClient.printf("Uptime : %lu:%02lu\n", s / 60, s % 60);
      telnetClient.printf("Heap   : %u bytes\n", ESP.getFreeHeap());
      telnetClient.printf("RSSI   : %d dBm\n", WiFi.RSSI());
      telnetClient.printf("IP     : %s\n", WiFi.localIP().toString().c_str());
      telnetClient.println("--------------");
    }
  }

  // OTA 処理（最優先・毎ループ必ず呼ぶ）
  if (s_otaReady) {
    ArduinoOTA.handle();
  }
}
