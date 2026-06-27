#include "Config.h"
#include "Display.h"
#include "Storage.h"
#include "Animations.h"
#include "Content.h"
#include "Inbox.h"
#include "BleStack.h"
#include "BleGatt.h"
#include "BleBroadcast.h"
#include "OTA_Handler.h"
#include "Fluid.h"
#include "Settings.h"
#include <ArduinoJson.h>

// ============================================================
// Config.h extern 変数の実体定義
//   BLE "settings" JSON で書き換え可能
// ============================================================
uint16_t MOTION_HUE       = 104; // 色相 0–360（度）。turnie4=104 (黄)
uint8_t MOTION_BRIGHTNESS = 50;
uint8_t MOTION_ANIM       = 0;   // 0=Ripple / 1=DiagonalWave
char    DEVICE_NAME[DEVICE_NAME_MAX] = {};  // 起動時に BLE_DEVICE_NAME で初期化

// ---------------------------------------------------------
// 受信メッセージのキュー
//   BT タスクから直接 Display や Animations を触ると
//   タスク競合で誤動作するため、フラグだけ立てて
//   メインタスク（loop）側で処理する
// ---------------------------------------------------------
static String        receivedRawJson      = "";
static unsigned long g_lastRxTime         = 0;
static String        g_lastSavedJson      = ""; // 直前に inbox へ保存した内容（30秒の重複保存ガード用）
static unsigned long g_lastSavedTime      = 0; // 直前に inbox へ保存した時刻（millis()）
static bool          newMessageFlag        = false;
static bool          g_needReply          = false;
static Content::Data parsedContent;
static bool          showContentAfterAnim = false;

// ---------------------------------------------------------
// 受信履歴の閲覧モード（USER ボタン）
//   長押しで開始/終了（DiagonalWave 演出）、一回押しで次の履歴へ。
//   無操作が BROWSE_TIMEOUT_MS 続いたら自動で通常フローへ戻る。
// ---------------------------------------------------------
static bool          g_browseMode        = false;
static int           g_browseIndex       = 0;     // Inbox の index（0=最古, size-1=最新）
static unsigned long g_browseLastActive  = 0;     // 最後にボタン操作した時刻

// ---------------------------------------------------------
// 待機画面のピクチャ確認モード（USER ボタン短押し）
//   待機中は流体シミュレーションを表示する。USER ボタンを短押しすると
//   自分の設定したピクチャを表示して確認できる。もう一度短押しするか、
//   PREVIEW_TIMEOUT_MS 無操作が続くと自動的に流体表示へ戻る。
// ---------------------------------------------------------
static const unsigned long PREVIEW_TIMEOUT_MS = 10000;  // 10 秒で流体へ自動復帰
static bool          g_previewMode       = false;
static unsigned long g_previewUntil      = 0;

// ---------------------------------------------------------
// コンテンツをマトリクスに表示する（テキスト・画像共通）
// ---------------------------------------------------------
void showContent(const Content::Data& data, bool isReceived = false) {
    if (data.kind == Content::Kind::Text) {
        // 自分のテキストなら無限ループ、受信したテキストなら1回のみ
        Display::showText(data.text.c_str(), TEXT_SCROLL_DELAY_MS, !isReceived);
    } else if (data.kind == Content::Kind::Image) {
        // 受信したデータのときのみアニメーション（1ピクセルずつ表示）を有効にする
        // 自分のデータのときは無期限表示 (ULONG_MAX) にする
        unsigned long duration = isReceived ? data.durationMs : ULONG_MAX;
        Display::showImage(data.rgb, sizeof(data.rgb), duration, isReceived);
    }
}

// ---------------------------------------------------------
// 閲覧モード：現在の g_browseIndex の受信履歴を表示する。
//   パースできない履歴は古い方へ読み飛ばす。
//   履歴が空（または全て無効）なら "no data" を表示する。
// ---------------------------------------------------------
void showBrowseItem() {
    int n = Inbox::size();
    if (n <= 0) {
        Display::showText("no data", TEXT_SCROLL_DELAY_MS, true, Animations::motionHueColor());
        return;
    }

    // 有効な履歴が見つかるまで最大 n 件、古い方へ読み飛ばす
    for (int tries = 0; tries < n; tries++) {
        Inbox::Item item;
        Content::Data data;
        if (Inbox::get(g_browseIndex, item) && Content::parse(item.json, data)) {
            if (data.kind == Content::Kind::Text) {
                // 次の操作までループスクロール
                Display::showText(data.text.c_str(), TEXT_SCROLL_DELAY_MS, true);
            } else if (data.kind == Content::Kind::Image) {
                // 1ピクセルずつ表示し、次の操作まで保持（ULONG_MAX）
                Display::showImage(data.rgb, sizeof(data.rgb), ULONG_MAX, true);
            }
            return;
        }
        // 無効な履歴 → 1つ古い方へ（末尾は最新へループ）
        g_browseIndex = (g_browseIndex - 1 + n) % n;
    }

    // 有効な履歴が1件も無かった
    Display::showText("no data", TEXT_SCROLL_DELAY_MS, true, Animations::motionHueColor());
}

// 閲覧モードを開始する（最新の履歴から表示）
void enterBrowseMode() {
    Animations::startDiagonalWave();   // 遷移演出（ブロッキング）
    g_browseMode       = true;
    g_browseLastActive = millis();
    int n = Inbox::size();
    g_browseIndex = (n > 0) ? (n - 1) : 0;   // 最新
    showBrowseItem();
    Serial.println("  [Browse] enter");
}

// 閲覧モードを終了して通常フローへ戻す
void exitBrowseMode() {
    Animations::startDiagonalWave();   // 遷移演出（ブロッキング）
    g_browseMode = false;
    Display::clear();
    Serial.println("  [Browse] exit");
}

// ピクチャ確認モードの開始/終了をトグルする（USER ボタン短押し）
//   開始：自分のピクチャを表示し、10秒のタイマーを張る
//   終了：流体表示へ戻す
void togglePreview() {
    if (!g_previewMode) {
        g_previewMode  = true;
        g_previewUntil = millis() + PREVIEW_TIMEOUT_MS;
        Display::clear();
        const Content::Data& own = Content::own();
        if (own.kind == Content::Kind::Image) {
            Display::showImageFadeIn(own.rgb, sizeof(own.rgb), ULONG_MAX);
        } else {
            showContent(own);   // テキストなどは従来どおり表示
        }
        Serial.println("  [Preview] show own picture");
    } else {
        g_previewMode = false;
        Display::clear();              // 流体表示は loop の待機処理で再開する
        Serial.println("  [Preview] back to fluid");
    }
}

// 1つ古い履歴へ進む（最古の次は最新へループ）
void browseNext() {
    int n = Inbox::size();
    if (n <= 0) return;
    g_browseIndex = (g_browseIndex - 1 + n) % n;
    g_browseLastActive = millis();
    showBrowseItem();
}

// ---------------------------------------------------------
// 他のデバイスからメッセージを受信したときに呼ばれる（BT タスク）
//   ここでは保存とフラグ設定だけ行う。
//   表示・アニメーションは loop() 内でメインタスクが処理する。
// ---------------------------------------------------------
void onMessageReceived(const uint8_t* data, size_t len) {
    if (len == 0) return;

    String json;
    if (data[0] == 'I') {
        // 画像はバイナリで届くため JSON に復元する
        if (len < 193) return; // 'I' + 192 bytes
        Content::Data imgData;
        imgData.kind = Content::Kind::Image;
        memcpy(imgData.rgb, data + 1, 192);
        imgData.durationMs = 5000;
        json = Content::serialize(imgData);
        Serial.println("  [RX] received image binary -> mapped to json");
    } else if (data[0] == 'T') {
        json = String((const char*)(data + 1), len - 1);
        Serial.println("  [RX] received text: " + json);
    } else {
        // 旧フォーマットや TypeByte がない場合のフォールバック
        json = String((const char*)data, len);
        Serial.println("  [RX] received unknown: " + json);
    }

    // 受信クールダウン：一度受信を処理したら、RX_IGNORE_MS の間は
    //   内容に関わらず後続の受信を全て無視する。
    //   近接中は同一広告が高頻度で連続して届くため、これをしないと
    //   ripple 連発・inbox の重複保存・返信の嵐が起きてバグったように見える。
    //   相手が離れて RX_IGNORE_MS 以上経てば、次の接触をまた1回だけ受け付ける。
    //   （g_lastRxTime はクールダウン中の無視パケットでは更新しない＝固定時間で復帰する）
    if (g_lastRxTime != 0 && (millis() - g_lastRxTime < RX_IGNORE_MS)) {
        return; // クールダウン中：完全に無視
    }
    g_lastRxTime = millis();

    // ここに来るのはクールダウン明けの「1発目」だけ。
    // 同一内容の 5 分リジェクト：
    //   直前に受け付けたのと同じ内容が RX_SAME_REJECT_MS（5分）以内に届いた場合は、
    //   完全にリジェクトする（表示も保存も返信もしない）。
    //   5 分を超えていれば、同じ内容でも改めて受け付ける。
    if (json == g_lastSavedJson
        && g_lastSavedTime != 0
        && (millis() - g_lastSavedTime < RX_SAME_REJECT_MS)) {
        Serial.println("  [RX] same content within 5min -> rejected (no display/save/reply)");
        return;
    }

    Inbox::append(json);
    g_lastSavedJson = json;
    g_lastSavedTime = millis();

    receivedRawJson = json;
    newMessageFlag   = true;
    g_needReply      = true;
}

// ---------------------------------------------------------
// setup: 起動時に一度だけ実行される
// ---------------------------------------------------------
void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n--- turnie_device_v2 ---");

    pinMode(PIN_BTN_USER, INPUT_PULLUP);
    pinMode(PIN_BTN_BOOT, INPUT_PULLUP);

    Display::init();
    if (!Storage::init()) {
        Serial.println("  [Setup] Storage init failed.");
    }
    Content::init();
    Inbox::init();

    // setting.json から前回の設定（hue, brightness, motion, name）をロード
    strncpy(DEVICE_NAME, BLE_DEVICE_NAME, DEVICE_NAME_MAX - 1); // デフォルト名で初期化
    Settings::loadFromStorage(false);

    BleStack::init();
    BleGatt::init();

    BleBroadcast::setOnReceive(onMessageReceived);
    // コンテンツの種類に応じて初期アドバタイズデータを設定
    // 画像はJSONに変換すると664バイトに肥大化してエラーになるため、バイナリで設定する
    {
        const Content::Data& own = Content::own();
        if (own.kind == Content::Kind::Image) {
            BleBroadcast::setInitialImageRgb(own.rgb, sizeof(own.rgb));
        } else if (own.kind == Content::Kind::Text) {
            BleBroadcast::setInitialData(Content::serialize(own));
        }
        // Kind::None の場合はデバイス名のみの初期値が使われる
    }
    BleBroadcast::init();

    // 待機画面の流体シミュレーション（QMI8658 + 受信データの粒）を初期化
    Fluid::init();

    Animations::startMotion();
}

// ---------------------------------------------------------
// loop: 繰り返し実行される
// ---------------------------------------------------------
void loop() {
    // setting.json の内容を 5 秒ごとにシリアルへ出力（デバッグ用）。
    //   どのモードでも必ず動くよう loop() の先頭で処理する。
    static unsigned long lastSettingDump = 0;
    if (millis() - lastSettingDump >= 5000) {
        lastSettingDump = millis();
        String settingJson = Storage::loadJson(SD_SETTING_JSON);
        if (settingJson.isEmpty()) {
            Serial.println("  [setting.json] (empty or not found)");
        } else {
            Serial.println("  [setting.json] " + settingJson);
        }
    }

    // BOOT ボタンのハンドリング
    static unsigned long bootPressedAt = 0;
    static bool resetTriggered = false;

    if (digitalRead(PIN_BTN_BOOT) == LOW) {
        if (bootPressedAt == 0) {
            bootPressedAt = millis();
            resetTriggered = false;
        } else if (!resetTriggered && (millis() - bootPressedAt >= 5000)) {
            // 5秒間長押し：すべてのJSONデータをリセット
            resetTriggered = true;

            // 紫で2秒点灯
            Display::fillColor(128, 0, 128); // 紫 (R=128, G=0, B=128)
            delay(2000);
            Display::clear();

            // JSONデータをリセット（LittleFS上のファイル削除）
            Storage::clearAll();

            // インメモリの変数も初期化
            MOTION_HUE        = 104; // デフォルト (黄, 色相0–360)
            MOTION_BRIGHTNESS = 50;
            Display::setBrightness(MOTION_BRIGHTNESS); // HWマトリクスへ即時反映
            MOTION_ANIM       = 0;
            strncpy(DEVICE_NAME, BLE_DEVICE_NAME, DEVICE_NAME_MAX - 1);
            DEVICE_NAME[DEVICE_NAME_MAX - 1] = '\0';
            g_lastSavedJson   = ""; // 重複保存ガードのキャッシュもクリア
            g_lastSavedTime   = 0;

            // 各種データのインメモリ状態もリロード/初期化
            Content::init();
            Inbox::init();
            Fluid::syncFromInbox();  // 受信件数 0 に合わせて粒を作り直す

            // 接続デバイス名や初期アドバタイズデータを更新・再送信
            BleBroadcast::updateGattName();
            {
                const Content::Data& own = Content::own();
                if (own.kind == Content::Kind::Image) {
                    BleBroadcast::setInitialImageRgb(own.rgb, sizeof(own.rgb));
                } else if (own.kind == Content::Kind::Text) {
                    BleBroadcast::setInitialData(Content::serialize(own));
                }
            }

            Serial.println("  [Reset] All JSON data and settings reset completed.");
        }
    } else {
        if (bootPressedAt != 0) {
            unsigned long pressDuration = millis() - bootPressedAt;
            bootPressedAt = 0;
            // 5秒未満かつ200ms以上の押下で OTA モードへ移行
            if (!resetTriggered && pressDuration > 200 && !isDebugMode()) {
                startDebugMode();
            }
        }
    }

    // USER ボタンのハンドリング（受信履歴の閲覧モード）
    //   長押し：閲覧モードの開始/終了（DiagonalWave 演出付き）
    //   一回押し：閲覧モード中なら次の履歴へ
    static unsigned long userPressedAt = 0;
    static bool          userLongFired = false;

    if (!isDebugMode()) {
        if (digitalRead(PIN_BTN_USER) == LOW) {
            if (userPressedAt == 0) {
                userPressedAt = millis();
                userLongFired = false;
            } else if (!userLongFired && (millis() - userPressedAt >= BROWSE_LONG_PRESS_MS)) {
                // 長押し確定（押している最中に1回だけ発火）
                userLongFired = true;
                if (!g_browseMode) {
                    enterBrowseMode();
                } else {
                    exitBrowseMode();
                }
            }
        } else {
            if (userPressedAt != 0) {
                unsigned long pressDuration = millis() - userPressedAt;
                userPressedAt = 0;
                // 長押しでなく、チャタリング閾値を超えた離しなら「一回押し」
                if (!userLongFired && pressDuration > BUTTON_DEBOUNCE_MS) {
                    if (g_browseMode) {
                        browseNext();          // 閲覧モード中：次の履歴へ
                    } else {
                        togglePreview();        // 通常時：ピクチャ確認 ↔ 流体
                    }
                }
                userLongFired = false;
            }
        }
    }

    // OTA モード中は通常処理をスキップ
    if (isDebugMode()) {
        handleDebugMode();
        return;
    }

    // 閲覧モード中：表示更新だけ行い、通常の受信/自分のコンテンツ表示は止める。
    //   無操作が続いたら自動的に通常フローへ戻る。
    if (g_browseMode) {
        if (millis() - g_browseLastActive >= BROWSE_TIMEOUT_MS) {
            exitBrowseMode();
        } else {
            Display::tick();
        }
        delay(LOOP_TICK_MS);
        return;
    }

    // アニメーション中はアニメーションを優先する
    if (Animations::isRunning()) {
        Animations::tick();
        delay(LOOP_TICK_MS);
        return;
    }

    // アニメーション終了直後：受信コンテンツを表示する
    if (showContentAfterAnim) {
        showContentAfterAnim = false;
        showContent(parsedContent, true); // 受信データとして表示（アニメーション有効）
    }

    // 受信メッセージを処理する（全てメインタスクで実行）
    if (newMessageFlag) {
        newMessageFlag = false;
        g_previewMode = false;    // 受信コンテンツを優先（ピクチャ確認中なら解除）
        Fluid::syncFromInbox();   // 受信件数が増えた → 粒の数と色を更新
        if (Content::parse(receivedRawJson, parsedContent)) {
            showContentAfterAnim = true;
        }
        // 受信したら即座に返信（isShowing() を待たない）
        if (g_needReply) {
            g_needReply = false;
            Content::Data ownData = Content::own();
            if (ownData.kind == Content::Kind::Text) {
                BleBroadcast::send(Content::serialize(ownData));
            } else if (ownData.kind == Content::Kind::Image) {
                BleBroadcast::sendImageRgb(ownData.rgb, sizeof(ownData.rgb));
            }
            Serial.println("  [TX] Sent own content back to partner");
        }
        Animations::startRipple();
        delay(LOOP_TICK_MS);
        return;
    }

    Display::tick();
    BleGatt::tick();

    // ピクチャ確認モード：自分のピクチャを表示し、10秒で流体へ自動復帰。
    //   （もう一度短押しした場合は togglePreview() 側で即座に解除される）
    if (g_previewMode) {
        if (millis() >= g_previewUntil) {
            g_previewMode = false;
            Display::clear();
            // 下の流体表示へフォールスルー
        } else {
            delay(LOOP_TICK_MS);
            return;   // ピクチャ表示中（Display が保持）
        }
    }

    // 待機画面：流体シミュレーションを表示する。
    //   受信コンテンツ等を表示中（isShowing）はそれを優先し、流体は描かない。
    if (!Display::isShowing()) {
        Fluid::tick();
    }

    delay(LOOP_TICK_MS);
}
