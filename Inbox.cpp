#include "Inbox.h"
#include "Storage.h"
#include "Config.h"

#include <ArduinoJson.h>

// ============================================================
// リングバッファの内部状態
// ============================================================

// メッセージを格納する固定サイズの配列
static Inbox::Item g_buffer[INBOX_CAPACITY];

// 次に書き込む位置（0 〜 INBOX_CAPACITY-1 を循環する）
static int g_writeIndex = 0;

// 現在保存されているメッセージの件数
static int g_count = 0;

// ============================================================
// 内部ヘルパー: 現在のメモリ上のバッファを SD の JSON 配列に保存する
// ============================================================
static void saveInboxToSD() {
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    // 最古のものから最新のものまで順番に JSON 配列に追加する
    for (int i = 0; i < g_count; i++) {
        Inbox::Item item;
        if (Inbox::get(i, item)) {
            // 受信メッセージのJSON文字列を再パースしてオブジェクトとして格納
            JsonDocument itemDoc;
            if (deserializeJson(itemDoc, item.json) == DeserializationError::Ok) {
                arr.add(itemDoc);
            } else {
                // パース失敗した場合は文字列としてそのまま追加
                arr.add(item.json);
            }
        }
    }

    String output;
    serializeJson(doc, output);
    Storage::saveJson(SD_INBOX_JSON, output);
}

// ============================================================
// Public API
// ============================================================
namespace Inbox {

bool init() {
    g_writeIndex = 0;
    g_count      = 0;

    String json = Storage::loadJson(SD_INBOX_JSON);
    if (json.isEmpty()) {
        Serial.println("  [Inbox] No saved inbox found on SD.");
        return false;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
        Serial.println("  [Inbox] JSON parse error on load: " + String(err.c_str()));
        return false;
    }

    JsonArray arr = doc.as<JsonArray>();
    if (arr.isNull()) {
        Serial.println("  [Inbox] Loaded JSON is not an array.");
        return false;
    }

    // JSON 配列からメモリ上のリングバッファにロード（最大 INBOX_CAPACITY 件）
    int loadedCount = 0;
    for (JsonVariant val : arr) {
        if (loadedCount >= (int)INBOX_CAPACITY) break;

        String itemJson;
        serializeJson(val, itemJson);

        g_buffer[g_writeIndex].json       = itemJson;
        g_buffer[g_writeIndex].receivedAt = millis();

        g_writeIndex = (g_writeIndex + 1) % INBOX_CAPACITY;
        loadedCount++;
    }
    g_count = loadedCount;

    Serial.printf("  [Inbox] Loaded %d items from SD.\n", g_count);
    return true;
}

void append(const String& json) {
    // バッファの書き込み位置にメッセージを保存する
    g_buffer[g_writeIndex].json       = json;
    g_buffer[g_writeIndex].receivedAt = millis();

    // 書き込み位置を 1 つ進める（最後まで来たら 0 に戻る）
    g_writeIndex = (g_writeIndex + 1) % INBOX_CAPACITY;

    // 件数を増やす（上限を超えない）
    if (g_count < INBOX_CAPACITY) {
        g_count++;
    }

    // SD カードの JSON ファイルに上書き保存する
    saveInboxToSD();

    Serial.printf("  [Inbox] saved (%d/%d)\n", g_count, INBOX_CAPACITY);
}

int size() {
    return g_count;
}

bool get(int index, Item& out) {
    // 範囲チェック
    if (index < 0 || index >= g_count) {
        return false;
    }

    // index=0 が最も古いメッセージになるよう、読み出し位置を計算する
    // 書き込み位置から g_count 個さかのぼった場所が最古のメッセージ
    int oldestIndex = (g_writeIndex - g_count + INBOX_CAPACITY) % INBOX_CAPACITY;

    // そこから index 個分進めた位置が目的のメッセージ
    int targetIndex = (oldestIndex + index) % INBOX_CAPACITY;

    out = g_buffer[targetIndex];
    return true;
}

void clear() {
    g_writeIndex = 0;
    g_count      = 0;
    
    // SD カードの内容も空にして保存
    saveInboxToSD();
    Serial.println("  [Inbox] cleared both RAM and SD");
}

} // namespace Inbox
