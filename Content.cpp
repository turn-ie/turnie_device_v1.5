#include "Content.h"
#include "Storage.h"

#include <ArduinoJson.h>

// このファイル内だけで使う変数
static Content::Data g_own;

namespace Content {

// ============================================================
// 初期化・永続化
// ============================================================
bool init() {
    g_own = Data(); // メモリ状態をリセット
    String json = Storage::loadJson(SD_OWN_JSON);

    if (json.length() == 0) {
        // SD 未挿入または data.json なし
        // g_own は Kind::None のまま。スマホから書き込まれるまで何も表示しない。
        Serial.println("  [Content] data.json not found. waiting for phone input.");
        return false;
    }

    if (!parse(json, g_own)) {
        Serial.println("  [Content] parse failed. waiting for phone input.");
        return false;
    }

    String kindName = (g_own.kind == Kind::Image) ? "image" : "text";
    Serial.println("  [Content] own loaded: kind=" + kindName);
    return true;
}

const Data& own() {
    return g_own;
}

bool saveOwn(const Data& data) {
    g_own = data;
    String json = serialize(data);
    return Storage::saveJson(SD_OWN_JSON, json);
}

// ============================================================
// JSON パース
//   対応フォーマット:
//     テキスト: {"flag":"text",  "text":"Hello!"}
//     画像:     {"flag":"emoji", "rgb":[192個の数値]}
//               {"flag":"photo", "rgb":[192個の数値]}
// ============================================================
bool parse(const String& json, Data& out) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, json);

    if (err) {
        Serial.println("  [Content] JSON parse error: " + String(err.c_str()));
        return false;
    }

    // "flag" または "kind" を取得（既存の flag:image と新フォーマットに対応）
    String flag = doc["flag"] | doc["kind"] | "";
    out.durationMs = (unsigned long)RX_DISPLAY_HOLD_MS;

    if (flag == "text") {
        String text = doc["text"].as<String>();
        if (text.length() == 0) {
            Serial.println("  [Content] text: empty string");
            return false;
        }
        out.text = text;
        out.kind = Kind::Text;
        return true;
    }

    if (flag == "emoji" || flag == "photo" || flag == "image") {
        JsonArray rgbArray = doc["rgb"].as<JsonArray>();
        if (rgbArray.isNull() || rgbArray.size() < (size_t)MATRIX_RGB_BYTES) {
            Serial.println("  [Content] image: rgb array missing or too short");
            return false;
        }
        for (int i = 0; i < MATRIX_RGB_BYTES; i++) {
            out.rgb[i] = (uint8_t)rgbArray[i];
        }
        out.kind = Kind::Image;
        return true;
    }

    Serial.println("  [Content] unknown flag: " + flag);
    return false;
}

// ============================================================
// JSON シリアライズ
//   デバイス間ブロードキャスト用。アプリ側と同じ形式で出力する。
// ============================================================
String serialize(const Data& data) {
    // Kind::None のときは空文字を返す
    // → BleBroadcast はデバイス名をデフォルトとして送信する
    if (data.kind == Kind::None) return "";

    JsonDocument doc;

    if (data.kind == Kind::Image) {
        doc["flag"] = "image";
        JsonArray rgbArray = doc["rgb"].to<JsonArray>();
        for (int i = 0; i < MATRIX_RGB_BYTES; i++) {
            rgbArray.add(data.rgb[i]);
        }
    } else {
        doc["flag"] = "text";
        doc["text"] = data.text;
    }

    String result;
    serializeJson(doc, result);
    return result;
}

} // namespace Content
