#include "BleGatt.h"
#include "BleBroadcast.h"
#include "Config.h"
#include "Content.h"
#include "Display.h"
#include "Storage.h"
#include "Animations.h"
#include "Settings.h"
#include "Fluid.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>

// ============================================================
// 内部状態
// ============================================================
static BLECharacteristic* g_pTxChar     = nullptr;
static String             g_pendingData = "";
static bool               g_pendingReady  = false;
static bool               g_pendingIsJson = false;  // true=JSON / false=テキスト直接表示
static constexpr unsigned long PHONE_WRITE_PREVIEW_MS = 3000;
static constexpr size_t GATT_NOTIFY_CHUNK_BYTES = 20;

// スマホ探索用 GATT 広告（handle 1）の自己回復ウォッチドッグ。
// 接続時に広告は自動停止するため、未接続の間は定期的に広告を再アサートして
// 「切断後の再開が失敗したまま探索不能になる」状態を自動復旧させる。
static volatile bool      g_phoneConnected = false;
static constexpr unsigned long GATT_ADV_WATCHDOG_MS = 3000;

static bool notifyText(const String& message) {
    if (g_pTxChar == nullptr || message.isEmpty()) return false;

    for (size_t offset = 0; offset < message.length(); offset += GATT_NOTIFY_CHUNK_BYTES) {
        String chunk = message.substring(offset, offset + GATT_NOTIFY_CHUNK_BYTES);
        g_pTxChar->setValue(chunk.c_str());
        g_pTxChar->notify();
        delay(10);
    }
    return true;
}

static int brightnessPercentFromHardware(uint8_t brightness) {
    return ((int)brightness * 100 + 127) / 255;
}

static String buildSettingsJson() {
    String settingsJson = Storage::loadJson(SD_SETTING_JSON);
    if (!settingsJson.isEmpty()) {
        JsonDocument doc;
        if (deserializeJson(doc, settingsJson) == DeserializationError::Ok) {
            return settingsJson;
        }
        Serial.println("  [BleGatt] setting.json is invalid, sending current settings");
    }

    JsonDocument doc;
    doc["flag"]       = "settings";
    doc["hue"]        = MOTION_HUE;
    doc["brightness"] = brightnessPercentFromHardware(MOTION_BRIGHTNESS);
    doc["motion"]     = Animations::motionEnumToString(MOTION_ANIM);
    doc["name"]       = DEVICE_NAME[0] ? DEVICE_NAME : BLE_DEVICE_NAME;
    doc["hometown"]   = HOMETOWN;

    String out;
    serializeJson(doc, out);
    return out;
}

static bool sendSettingsToPhone() {
    String settingsJson = buildSettingsJson();
    Serial.println("  [BleGatt] sending settings: " + settingsJson);
    return notifyText(settingsJson);
}

// ============================================================
// 受信コールバック
//   GATT で送られてきた JSON データを受け取る
// ============================================================
class WriteCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pChar) override {
        String received = String(pChar->getValue().c_str());
        if (received.isEmpty()) return;

        static String buffer = "";
        buffer += received;

        // JSON モード：末尾 '}' が届いたら 1 件完了として処理
        if (buffer.startsWith("{") && buffer.endsWith("}")) {
            g_pendingData   = buffer;
            g_pendingIsJson = true;
            g_pendingReady  = true;
            buffer          = "";
            Serial.println("  [BleGatt] JSON received: " + g_pendingData);
        }
    }
};

// ============================================================
// 接続コールバック
// ============================================================
class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) override {
        g_phoneConnected = true;
        Serial.println("  [BleGatt] phone connected");
    }

    void onDisconnect(BLEServer* pServer) override {
        // 即時再開はベストエフォート（コールバック文脈なので失敗しうる）。
        // 失敗しても tick() のウォッチドッグが未接続を検知して再アサートする。
        g_phoneConnected = false;
        Serial.println("  [BleGatt] phone disconnected. restarting advertising...");
        BleBroadcast::restartGattAdv();
    }
};

// ============================================================
// Public API
// ============================================================
namespace BleGatt {

bool init() {
    BLEServer* server = BLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks());

    BLEService* service = server->createService(GATT_SVC_UUID);

    BLECharacteristic* rxChar = service->createCharacteristic(
        GATT_RX_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    rxChar->setCallbacks(new WriteCallback());

    g_pTxChar = service->createCharacteristic(
        GATT_TX_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    g_pTxChar->addDescriptor(new BLE2902());

    service->start();

    // GATT 広告は BleBroadcast の handle 1（legacy 互換 connectable）で発信される。
    // ここでは GATT サービスを登録するだけ。
    Serial.println("  [BleGatt] service ready");
    return true;
}

void tick() {
    // --- GATT 広告ウォッチドッグ ---
    // 未接続なのに広告が止まっている（切断時の再開失敗など）と探索不能になるため、
    // 未接続の間は一定間隔で handle 1 の広告を再アサートして自動復旧させる。
    {
        static unsigned long lastAdvCheck = 0;
        unsigned long now = millis();
        if (!g_phoneConnected && now - lastAdvCheck >= GATT_ADV_WATCHDOG_MS) {
            lastAdvCheck = now;
            BleBroadcast::restartGattAdv();
        }
    }

    if (!g_pendingReady) return;

    String data    = g_pendingData;
    g_pendingReady = false;

    // ----------------------------------------------------------
    // flag: "settings" の場合は設定変数のみを更新して終わり
    // ----------------------------------------------------------
    {
        JsonDocument settingsDoc;
        if (deserializeJson(settingsDoc, data) == DeserializationError::Ok) {
            String flag = settingsDoc["flag"] | "";
            if (flag == "get_settings") {
                sendSettingsToPhone();
                return;
            }
            if (flag == "settings") {
                // スマホから送られてきた settings の内容を丸ごと SD (setting.json) に保存する。
                // hue / brightness / name / hometown など、含まれる項目をそのまま保持する。
                {
                    String settingsJson;
                    serializeJson(settingsDoc, settingsJson);
                    if (Storage::saveJson(SD_SETTING_JSON, settingsJson)) {
                        Serial.println("  [BleGatt] saved settings to SD: " + settingsJson);
                        // ripple の前に設定を反映 → 新しい設定色で ripple が再生される
                        Settings::loadFromStorage(true);
                        Animations::startRipple();
                        Display::clear();
                        notifyText("saved");
                    }
                }

                return; // settings 処理完了。コンテンツは保存しない
            }
        }
    }

    // コンテンツをパースして保存
    Content::Data content;
    if (!Content::parse(data, content)) return;

    // 自分のコンテンツとして更新（メモリ + SD）
    Content::saveOwn(content);
    Serial.println("  [BleGatt] saved as own content");
    notifyText("saved");

    // 待機（流体）の自分の粒(index 0)の色を更新（戻ったとき古い色にならないように）
    Fluid::syncFromInbox();

    // プレビュー：画像・文字とも PHONE_WRITE_PREVIEW_MS だけ表示し、自動で待機へ戻る
    if (content.kind == Content::Kind::Text) {
        Display::showText(content.text.c_str(), TEXT_SCROLL_DELAY_MS, true,
                          0xFFFF, PHONE_WRITE_PREVIEW_MS);
    } else if (content.kind == Content::Kind::Image) {
        Display::showImage(content.rgb, sizeof(content.rgb), PHONE_WRITE_PREVIEW_MS, false);
    }

    // 他のデバイスへブロードキャストする
    if (content.kind == Content::Kind::Image) {
        BleBroadcast::sendImageRgb(content.rgb, sizeof(content.rgb));
    } else {
        String json = Content::serialize(content);
        Serial.println("  [TX] " + json);
        BleBroadcast::send(json);
    }
}

} // namespace BleGatt
