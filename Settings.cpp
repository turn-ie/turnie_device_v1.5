#include "Settings.h"

#include "Animations.h"
#include "BleBroadcast.h"
#include "Config.h"
#include "Display.h"
#include "Storage.h"

#include <ArduinoJson.h>

namespace Settings {

bool loadFromStorage(bool updateGattName) {
    String settingJson = Storage::loadJson(SD_SETTING_JSON);
    if (settingJson.isEmpty()) {
        return false;
    }

    JsonDocument settingsDoc;
    DeserializationError err = deserializeJson(settingsDoc, settingJson);
    if (err != DeserializationError::Ok) {
        Serial.println("  [Settings] failed to parse setting.json");
        return false;
    }

    if (!settingsDoc["hue"].isNull()) {
        // 色相は 0–360（度）。範囲外はクランプする。
        int hue = settingsDoc["hue"].as<int>();
        if (hue < 0)   hue = 0;
        if (hue > 360) hue = 360;
        MOTION_HUE = (uint16_t)hue;
    }
    if (!settingsDoc["brightness"].isNull()) {
        // setting.json の brightness は 0–100。ハードウェアは 0–255 なので変換する。
        int b = settingsDoc["brightness"].as<int>();
        if (b < 0)   b = 0;
        if (b > 100) b = 100;
        MOTION_BRIGHTNESS = (uint8_t)(b * 255 / 100);
        Display::setBrightness(MOTION_BRIGHTNESS);
    }
    if (!settingsDoc["motion"].isNull()) {
        if (settingsDoc["motion"].is<const char*>()) {
            String motion = settingsDoc["motion"].as<String>();
            if (motion == "1") {
                MOTION_ANIM = 1;
            } else if (motion == "0") {
                MOTION_ANIM = 0;
            } else {
                MOTION_ANIM = Animations::motionStringToEnum(motion);
            }
        } else {
            MOTION_ANIM = (uint8_t)settingsDoc["motion"].as<int>();
        }
    }
    if (!settingsDoc["name"].isNull()) {
        String name = settingsDoc["name"].as<String>();
        strncpy(DEVICE_NAME, name.c_str(), DEVICE_NAME_MAX - 1);
        DEVICE_NAME[DEVICE_NAME_MAX - 1] = '\0';
        if (updateGattName) {
            BleBroadcast::updateGattName();
        }
    }
    if (!settingsDoc["hometown"].isNull()) {
        String hometown = settingsDoc["hometown"].as<String>();
        strncpy(HOMETOWN, hometown.c_str(), HOMETOWN_MAX - 1);
        HOMETOWN[HOMETOWN_MAX - 1] = '\0';
    }

    Serial.printf("  [Settings] loaded: hue=%d brightness=%d motion=%s name=%s hometown=%s\n",
                  MOTION_HUE,
                  MOTION_BRIGHTNESS,
                  Animations::motionEnumToString(MOTION_ANIM).c_str(),
                  DEVICE_NAME,
                  HOMETOWN);
    return true;
}

} // namespace Settings
