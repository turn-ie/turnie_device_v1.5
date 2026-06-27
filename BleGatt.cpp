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
            sendAck("saved");
        }
    }

    void sendAck(const char* msg) {
        if (g_pTxChar != nullptr) {
            g_pTxChar->setValue(msg);
            g_pTxChar->notify();
        }
    }
};

// ============================================================
// 接続コールバック
// ============================================================
class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) override {
        Serial.println("  [BleGatt] phone connected");
    }

    void onDisconnect(BLEServer* pServer) override {
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
            if (flag == "settings") {
                // 設定を SD (setting.json) に保存する
                {
                    JsonDocument outDoc;
                    outDoc["flag"]       = "settings";
                    outDoc["hue"]        = settingsDoc["hue"] | MOTION_HUE;
                    outDoc["brightness"] = settingsDoc["brightness"] | MOTION_BRIGHTNESS;
                    if (!settingsDoc["motion"].isNull()) {
                        outDoc["motion"] = settingsDoc["motion"];
                    } else {
                        outDoc["motion"] = Animations::motionEnumToString(MOTION_ANIM);
                    }
                    outDoc["name"]       = settingsDoc["name"] | DEVICE_NAME;
                    String settingsJson;
                    serializeJson(outDoc, settingsJson);
                    if (Storage::saveJson(SD_SETTING_JSON, settingsJson)) {
                        Serial.println("  [BleGatt] saved settings to SD: " + settingsJson);
                        // ripple の前に設定を反映 → 新しい設定色で ripple が再生される
                        Settings::loadFromStorage(true);
                        Animations::startRipple();
                        Display::clear();
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
