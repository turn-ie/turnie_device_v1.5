#pragma once
#include <Arduino.h>
#include "Config.h"

// =============================================================
// Content
//   デバイスが送受信する「コンテンツ」の定義と JSON パース。
//   コンテンツは Image（8x8 RGB バイト列）か Text のどちらか。
//
//   JSON フォーマット:
//     テキスト: { "flag": "text",  "text": "Hello!" }
//     画像:     { "flag": "emoji", "rgb": [R,G,B, ... 計192個] }
//               { "flag": "photo", "rgb": [R,G,B, ... 計192個] }
// =============================================================
namespace Content {

    enum class Kind { None, Image, Text };

    struct Data {
        Kind     kind        = Kind::None;
        uint8_t  rgb[MATRIX_RGB_BYTES] = {};   // Image の場合
        String   text        = "";             // Text の場合
        unsigned long durationMs = RX_DISPLAY_HOLD_MS;
    };

    // SD カードの SD_OWN_JSON から自分のコンテンツを読み込む
    bool init();

    // 自分のコンテンツへの参照（init() 後に有効）
    const Data& own();

    // 自分のコンテンツを更新して SD に保存する
    bool saveOwn(const Data& data);

    // JSON 文字列を Data に変換する。失敗時は false。
    bool parse(const String& json, Data& out);

    // Data を JSON 文字列に変換する
    String serialize(const Data& data);

}
