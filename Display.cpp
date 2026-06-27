#include "Display.h"
#include "Config.h"

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <climits>

// ============================================================
// このファイル内だけで使う変数・関数
// ============================================================

// NeoMatrix 本体
static Adafruit_NeoMatrix g_matrix(
    MATRIX_WIDTH, MATRIX_HEIGHT, PIN_MATRIX,
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
    NEO_GRB + NEO_KHZ800
);

// 表示タイマー
static unsigned long g_untilMs    = 0;      // この時刻になったら自動消灯
static bool          g_indefinite = false;  // 無期限表示フラグ

// テキストスクロールの状態
static bool          g_scrolling        = false;
static bool          g_scrollLoop       = false;
static String        g_scrollText       = "";
static uint16_t      g_scrollDelay      = 60;
static int           g_scrollX          = 0;
static int           g_scrollTextWidth  = 0;    // テキストの横幅（ピクセル）
static unsigned long g_scrollLastFrame  = 0;

// 1ピクセルずつ表示するノンブロッキングアニメーション
static bool          g_pixelAnim        = false;
static int           g_pixelAnimIdx     = 0;          // 次に描くピクセルのインデックス
static uint8_t       g_pixelAnimRgb[MATRIX_RGB_BYTES];// 画像バッファ
static unsigned long g_pixelAnimLast    = 0;
static unsigned long g_pixelAnimDurMs   = 0;          // アニメーション完了後の表示時間

static void drawRgbImage(const uint8_t* rgb) {
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
            int pixelIndex = (y * MATRIX_WIDTH + x) * 3;
            uint8_t r = rgb[pixelIndex];
            uint8_t g = rgb[pixelIndex + 1];
            uint8_t b = rgb[pixelIndex + 2];
            g_matrix.drawPixel(x, y, Display::Internal::color(r, g, b));
        }
    }
}

// 1文字あたりのピクセル幅（5x7 フォント + 1px スペース = 6px）
static int calcTextWidth(const char* text) {
    if (text == nullptr) return 0;
    return (int)strlen(text) * 6;
}

// テキストスクロールを 1 フレーム進める
static void advanceScrollFrame() {
    unsigned long now = millis();
    if (now - g_scrollLastFrame < g_scrollDelay) return;
    g_scrollLastFrame = now;

    g_matrix.fillScreen(0);
    g_matrix.setCursor(g_scrollX, 0);
    g_matrix.print(g_scrollText);
    g_matrix.show();

    g_scrollX--;

    // テキストが画面の左端を完全に出たら
    if (g_scrollX < -g_scrollTextWidth) {
        if (g_scrollLoop) {
            // ループ設定なら右端から再開
            g_scrollX = g_matrix.width();
        } else {
            // 1回だけなら終了して消灯し、0.5秒(500ms)の間隔を空ける
            g_scrolling = false;
            g_matrix.fillScreen(0);
            g_matrix.show();
            g_untilMs = millis() + 500;
        }
    }
}

// ============================================================
// Public API
// ============================================================
namespace Display {

void init() {
    g_matrix.begin();
    g_matrix.setBrightness(MOTION_BRIGHTNESS); // 全モード共通の明るさ（スマホ設定値）
    g_matrix.setRotation(3);       // 物理実装に合わせた回転
    g_matrix.setTextWrap(false);
    g_matrix.setTextColor(g_matrix.Color(255, 255, 255));
    g_matrix.fillScreen(0);
    g_matrix.show();

    g_untilMs    = 0;
    g_indefinite = false;
    g_scrolling  = false;
}

void setBrightness(uint8_t brightness) {
    g_matrix.setBrightness(brightness);
    g_matrix.show();
}

void tick() {
    if (g_scrolling) {
        // タイマーが設定されていれば、スクロール中でも期限切れで消灯し通常へ戻す
        if (!g_indefinite && g_untilMs != 0 && millis() >= g_untilMs) {
            clear();
            return;
        }
        advanceScrollFrame();
        return;
    }

    // 1ピクセルずつアニメーション（10ms ごとに 1px 描画）
    if (g_pixelAnim) {
        if (millis() - g_pixelAnimLast >= 10) {
            g_pixelAnimLast = millis();
            int x = g_pixelAnimIdx % MATRIX_WIDTH;
            int y = g_pixelAnimIdx / MATRIX_WIDTH;
            int i = g_pixelAnimIdx * 3;
            uint16_t color = Internal::color(g_pixelAnimRgb[i], g_pixelAnimRgb[i+1], g_pixelAnimRgb[i+2]);
            g_matrix.drawPixel(x, y, color);
            g_matrix.show();
            g_pixelAnimIdx++;
            if (g_pixelAnimIdx >= MATRIX_WIDTH * MATRIX_HEIGHT) {
                // アニメーション完了 → 表示タイマー開始
                g_pixelAnim  = false;
                if (g_pixelAnimDurMs == ULONG_MAX) {
                    // 無期限保持（閲覧モードなど。次の操作まで表示し続ける）
                    g_indefinite = true;
                    g_untilMs    = 0;
                } else {
                    g_indefinite = false;
                    g_untilMs    = millis() + g_pixelAnimDurMs;
                }
            }
        }
        return;
    }

    // タイマーが設定されていて期限切れなら消灯
    if (!g_indefinite && g_untilMs != 0 && millis() >= g_untilMs) {
        clear();
    }
}

void clear() {
    g_scrolling  = false;
    g_pixelAnim  = false;
    g_indefinite = false;
    g_untilMs    = 0;
    g_matrix.fillScreen(0);
    g_matrix.show();
}

void fillColor(uint8_t r, uint8_t g, uint8_t b) {
    g_scrolling  = false;
    g_indefinite = false;
    g_untilMs    = 0;
    g_matrix.fillScreen(Internal::color(r, g, b));
    g_matrix.show();
}

bool showImage(const uint8_t* rgb, size_t bytes, unsigned long durationMs, bool animated) {
    if (rgb == nullptr || bytes < (size_t)MATRIX_RGB_BYTES) return false;

    g_scrolling  = false;
    g_pixelAnim  = false;
    g_indefinite = false;
    g_untilMs    = 0;

    if (animated) {
        // ノンブロッキング：バッファにコピーして tick() で 1px ずつ描画
        memcpy(g_pixelAnimRgb, rgb, MATRIX_RGB_BYTES);
        g_pixelAnimIdx  = 0;
        g_pixelAnimLast = millis();
        g_pixelAnimDurMs = durationMs;
        g_pixelAnim     = true;
        g_matrix.fillScreen(0);
        g_matrix.show();
        return true;
    }

    // 通常（即時）表示
    drawRgbImage(rgb);
    g_matrix.show();

    // durationMs が ULONG_MAX のときは無期限表示
    if (durationMs == ULONG_MAX) {
        g_indefinite = true;
        g_untilMs    = 0;
    } else {
        g_indefinite = false;
        g_untilMs    = millis() + durationMs;
    }
    return true;
}

bool showImageFadeIn(const uint8_t* rgb, size_t bytes, unsigned long durationMs, uint16_t fadeMs) {
    if (rgb == nullptr || bytes < (size_t)MATRIX_RGB_BYTES) return false;

    g_scrolling  = false;
    g_pixelAnim  = false;
    g_indefinite = false;
    g_untilMs    = 0;

    const uint8_t targetBrightness = MOTION_BRIGHTNESS;
    const uint8_t steps = 14;
    const uint16_t frameDelay = (fadeMs / steps > 0) ? (fadeMs / steps) : 1;

    g_matrix.fillScreen(0);
    g_matrix.setBrightness(0);
    drawRgbImage(rgb);
    g_matrix.show();

    for (uint8_t step = 1; step <= steps; step++) {
        uint8_t brightness = (uint8_t)(((uint16_t)targetBrightness * step) / steps);
        g_matrix.setBrightness(brightness);
        g_matrix.show();
        delay(frameDelay);
    }

    g_matrix.setBrightness(targetBrightness);
    drawRgbImage(rgb);
    g_matrix.show();

    if (durationMs == ULONG_MAX) {
        g_indefinite = true;
        g_untilMs    = 0;
    } else {
        g_indefinite = false;
        g_untilMs    = millis() + durationMs;
    }
    return true;
}

void showText(const char* text, uint16_t frameDelayMs, bool loopForever,
              uint16_t color, unsigned long durationMs) {
    if (text == nullptr) return;

    g_scrollText      = text;
    g_scrollDelay     = frameDelayMs;
    g_scrollTextWidth = calcTextWidth(text);
    g_scrollLoop      = loopForever;
    g_scrollX         = g_matrix.width();  // 右端からスタート
    g_scrolling       = true;
    g_scrollLastFrame = millis();

    // durationMs > 0 のときだけ自動消灯タイマーを張る（tick() で処理）
    g_untilMs    = (durationMs > 0) ? (millis() + durationMs) : 0;
    g_indefinite = false;

    g_matrix.setTextColor(color);
    g_matrix.fillScreen(0);
    g_matrix.setCursor(g_scrollX, 0);
    g_matrix.print(g_scrollText);
    g_matrix.show();
}

void stopTextScroll() {
    g_scrolling = false;
}

bool isTextScrolling() {
    return g_scrolling;
}

void blockFor(unsigned long ms) {
    if (ms == 0) return;
    g_untilMs    = millis() + ms;
    g_indefinite = false;
}

bool isShowing() {
    if (g_scrolling)  return true;
    if (g_pixelAnim)  return true;
    if (g_indefinite) return true;
    if (g_untilMs != 0 && millis() < g_untilMs) return true;
    return false;
}

// Animations モジュール専用の低レベルアクセス
namespace Internal {
    Adafruit_NeoMatrix& matrix() { return g_matrix; }

    // パネルは R と G が物理的に入れ替わっているため入れ替えて渡す。
    // 色順の補正はここ 1 か所に集約する。
    uint16_t color(uint8_t r, uint8_t g, uint8_t b) {
        return g_matrix.Color(g, r, b);
    }
}

} // namespace Display
