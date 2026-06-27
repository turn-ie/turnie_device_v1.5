#pragma once
#include <Arduino.h>

// 前方宣言（ヘッダで Adafruit_NeoMatrix.h を include しない）
class Adafruit_NeoMatrix;

// =============================================================
// Display
//   8x8 LED マトリクス（NeoMatrix）の描画と表示タイマー管理。
//   上位レイヤー（Content / App）はこのモジュールを通してのみ
//   マトリクスを操作する。Animations だけは Internal::matrix() で
//   低レベルアクセスを許可。
// =============================================================
namespace Display {

    // ---- ライフサイクル ----
    void init();
    void tick();   // loop() から毎回呼ぶ（テキストスクロール更新と期限切れ処理）

    // ---- 描画 ----
    void clear();
    void fillColor(uint8_t r, uint8_t g, uint8_t b);

    // マトリクスの明るさを変更する（0-255）。
    void setBrightness(uint8_t brightness);

    // 8x8 RGB バイト列を表示する。
    // durationMs が経過したら tick() の中で自動消灯される（ULONG_MAX は無期限）。
    // animated=true の場合、ピクセルを 1 つずつ点灯するアニメーション付き。
    bool showImage(const uint8_t* rgb, size_t bytes,
                   unsigned long durationMs, bool animated = false);

    // 8x8 RGB バイト列をフェードインして表示する。
    bool showImageFadeIn(const uint8_t* rgb, size_t bytes,
                         unsigned long durationMs, uint16_t fadeMs = 420);

    // テキストをスクロール表示（ノンブロッキング、tick() でフレーム送り）。
    // color は文字色（16bit 565）。省略時は白。
    // durationMs > 0 のときは、スクロール中でもその時間が経過したら
    // 自動消灯して通常状態へ戻る（0 のときは従来どおりタイマー無し）。
    void showText(const char* text, uint16_t frameDelayMs, bool loopForever,
                  uint16_t color = 0xFFFF, unsigned long durationMs = 0);
    void stopTextScroll();
    bool isTextScrolling();

    // ---- 表示タイマー管理 ----
    // ms の間「占有中」フラグだけ立てる（描画はしない）。
    // 受信アニメ実行中に loop() の上書き処理を止める用途。
    void blockFor(unsigned long ms);

    // 何らかの表示が継続中か（タイマー有効中 or テキストスクロール中）
    bool isShowing();

    // ---- Animations / Fluid 専用の低レベルアクセス ----
    namespace Internal {
        Adafruit_NeoMatrix& matrix();

        // 論理 RGB → このパネル用 16bit カラー。R↔G の物理スワップ補正を
        // ここ 1 か所に集約（画像・塗りつぶし・流体・アニメすべてがここを通る）。
        uint16_t color(uint8_t r, uint8_t g, uint8_t b);
    }
}
