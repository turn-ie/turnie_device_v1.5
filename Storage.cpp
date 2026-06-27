#include "Storage.h"
#include "Config.h"

#include <LittleFS.h>

namespace {
    bool g_ready = false;
}

namespace Storage {

bool init() {
    // format_on_fail=true: 初回起動時やファイルシステム破損時に自動フォーマット
    if (!LittleFS.begin(true)) {
        Serial.println("  [Storage] LittleFS init failed");
        g_ready = false;
        return false;
    }
    g_ready = true;
    Serial.println("  [Storage] LittleFS ready");
    return true;
}

String loadJson(const char* path) {
    if (!g_ready || !path) return "";

    File f = LittleFS.open(path, "r");
    if (!f) {
        Serial.printf("  [Storage] open failed: %s\n", path);
        return "";
    }

    String out;
    out.reserve(f.size());
    while (f.available()) {
        out += (char)f.read();
    }
    f.close();
    return out;
}

bool saveJson(const char* path, const String& json) {
    if (!g_ready || !path) return false;

    File f = LittleFS.open(path, "w");
    if (!f) {
        Serial.printf("  [Storage] write open failed: %s\n", path);
        return false;
    }

    f.print(json);
    f.close();
    return true;
}

void appendLine(const char* path, const String& line) {
    if (!g_ready || !path) return;

    File f = LittleFS.open(path, "a");
    if (!f) {
        Serial.printf("  [Storage] append open failed: %s\n", path);
        return;
    }

    f.println(line);
    f.close();
}

void clearAll() {
    if (!g_ready) return;
    
    // ファイル自体は残したまま、中身を空（0バイト）にする
    const char* paths[] = {SD_OWN_JSON, SD_SETTING_JSON, SD_INBOX_JSON, SD_INBOX_LOG};
    for (const char* path : paths) {
        File f = LittleFS.open(path, "w");
        if (f) {
            f.close();
        }
    }
    Serial.println("  [Storage] Emptied the contents of all files");
}

} // namespace Storage
