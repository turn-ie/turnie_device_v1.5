#pragma once
#include <Arduino.h>

// =============================================================
// Storage
//   内部フラッシュ（LittleFS）への JSON 読み書きとログ追記。
//   上位レイヤー（Content / Inbox）はこのモジュール経由で
//   ファイルシステムにアクセスする。
// =============================================================
namespace Storage {

    // LittleFS を初期化する。失敗時は false を返す。
    bool init();

    // path のファイルを JSON 文字列として返す。失敗時は空文字列。
    String loadJson(const char* path);

    // json を path に上書き保存する。成功時 true。
    bool saveJson(const char* path, const String& json);

    // json 文字列を path に 1 行追記する（インボックスログ用）。
    void appendLine(const char* path, const String& line);

    // 全ての保存ファイルを削除する
    void clearAll();

}
