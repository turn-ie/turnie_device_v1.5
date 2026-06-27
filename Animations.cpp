#include "Animations.h"
#include "Config.h"
#include "Display.h"

#include <Adafruit_NeoMatrix.h>

// ============================================================
// HSV 色 → マトリクス用 16bit カラーに変換する
//   色順の補正（R↔G 入れ替え）は Display::Internal::color() に集約。
// ============================================================
static uint16_t hsvToMatrixColor(uint16_t hueDeg, uint8_t saturation,
                                 uint8_t brightness) {
  // 色相 0–360（度）を Adafruit の 16bit hue（0–65535）へ変換する
  uint16_t hue16 = (uint16_t)((uint32_t)hueDeg * 65536UL / 360UL);
  uint32_t color32 = Adafruit_NeoPixel::ColorHSV(hue16, saturation, brightness);
  uint8_t red = (color32 >> 16) & 0xFF;
  uint8_t green = (color32 >> 8) & 0xFF;
  uint8_t blue = color32 & 0xFF;
  return Display::Internal::color(red, green, blue);
}

// ============================================================
// Public API
// ============================================================
namespace Animations {

// 現在はノンブロッキングアニメーションが無い（Ripple / DiagonalWave は
// どちらもブロッキング実装）。loop() 側の互換のために常に「停止中」を返す。
bool isRunning() { return false; }

bool tick() { return false; }

// ============================================================
// startMotion：MOTION_ANIM に従ってアニメを選択して開始する
//   0 = Ripple / 1 = DiagonalWave
// ============================================================
void startMotion() {
  switch (MOTION_ANIM) {
  case 1:
    startDiagonalWave();
    break;
  default:
    startRipple();
    break; // 0 or unknown → Ripple
  }
}

// ============================================================
// Ripple：ガウシアン関数で複数リングを重ね合わせ、最後にフェードアウト。
//   ブロッキング実装（delay を使用）
// ============================================================
void startRipple() {
  Adafruit_NeoMatrix &mx = Display::Internal::matrix();

  const uint8_t LEVELS = 12;
  const float SPEED = 0.14f;
  const float SPACING = 0.85f;
  const float SIGMA = 0.55f;
  const int RINGS = 4;
  const float MAX_DIST = 4.95f;
  const float PERIOD = MAX_DIST + (RINGS - 1) * SPACING + 2.0f * SIGMA;

  const float cx = (MATRIX_WIDTH - 1) * 0.5f;
  const float cy = (MATRIX_HEIGHT - 1) * 0.5f;

  uint8_t savedBrightness = mx.getBrightness();
  mx.setBrightness(MOTION_BRIGHTNESS);

  // メインループ：t が PERIOD を超えるまでリングを描画する
  float t = 0.0f;
  while (t <= PERIOD) {
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
      for (int x = 0; x < MATRIX_WIDTH; x++) {
        float dx = (float)x - cx;
        float dy = (float)y - cy;
        float dist = sqrtf(dx * dx + dy * dy);

        // 複数リングのガウシアン振幅を足し合わせる
        float amp = 0.0f;
        for (int k = 0; k < RINGS; k++) {
          float r = t - (float)k * SPACING;
          float d = dist - r;
          amp += expf(-(d * d) / (2.0f * SIGMA * SIGMA));
        }
        if (amp > 1.0f)
          amp = 1.0f;

        // 段階的な明るさ（ちらつきを抑える）
        float stepped = floorf(amp * LEVELS) / LEVELS;

        // 中心ほど彩度が高く、外側は白っぽく
        float satf = 0.90f - 0.25f * (dist / MAX_DIST);
        if (satf < 0.0f)
          satf = 0.0f;
        if (satf > 1.0f)
          satf = 1.0f;

        // ガンマ補正をかけた明度（Adafruit 標準テーブルを使用）
        uint8_t V =
            Adafruit_NeoPixel::gamma8((uint8_t)(stepped * 0.9f * 255.0f));
        uint8_t S = (uint8_t)(satf * 255.0f + 0.5f);

        mx.drawPixel(x, y, hsvToMatrixColor(MOTION_HUE, S, V));
      }
    }
    mx.show();
    delay(20);
    t += SPEED;
  }

  // フェードアウト
  for (int b = (int)MOTION_BRIGHTNESS; b >= 0; b -= 4) {
    mx.setBrightness((uint8_t)b);
    mx.show();
    delay(16);
  }
  mx.fillScreen(0);
  mx.show();
  mx.setBrightness(savedBrightness);
}

// ============================================================
// DiagonalWave：左右端から中央へ収束する2つの光の波。
//   ブロッキング実装（delay を使用）
// ============================================================
void startDiagonalWave() {
  Adafruit_NeoMatrix &mx = Display::Internal::matrix();

  const uint8_t LEVELS = 12;
  const float SIGMA = 0.8f;
  const float SPEED = 0.18f;

  const float CENTER = (MATRIX_WIDTH - 1) * 0.5f;
  const float MARGIN = 2.5f;
  const float START_LEFT = -MARGIN;
  const float START_RIGHT = (MATRIX_WIDTH - 1) + MARGIN;
  const float HALF_DIST = CENTER + MARGIN;
  const float TOTAL_DIST = HALF_DIST * 2.0f;

  uint8_t savedBrightness = mx.getBrightness();
  mx.setBrightness(MOTION_BRIGHTNESS);

  float t = 0.0f;
  while (t <= TOTAL_DIST) {
    float posLeft = START_LEFT + t;
    float posRight = START_RIGHT - t;

    for (int y = 0; y < MATRIX_HEIGHT; y++) {
      for (int x = 0; x < MATRIX_WIDTH; x++) {
        float fx = (float)x;

        float distL = fx - posLeft;
        float ampL = expf(-(distL * distL) / (2.0f * SIGMA * SIGMA));

        float distR = fx - posRight;
        float ampR = expf(-(distR * distR) / (2.0f * SIGMA * SIGMA));

        float amp = ampL + ampR;
        if (amp > 1.0f)
          amp = 1.0f;

        float stepped = floorf(amp * LEVELS) / LEVELS;

        float satf = 0.90f - 0.25f * amp;
        if (satf < 0.0f)
          satf = 0.0f;
        if (satf > 1.0f)
          satf = 1.0f;

        uint8_t V =
            Adafruit_NeoPixel::gamma8((uint8_t)(stepped * 0.9f * 255.0f));
        V = (uint8_t)(((uint16_t)V * 250 + 127) / 255);

        uint8_t S = (uint8_t)(satf * 255.0f + 0.5f);

        mx.drawPixel(x, y, hsvToMatrixColor(MOTION_HUE, S, V));
      }
    }
    mx.show();
    delay(20);

    t += SPEED;
  }

  // フェードアウト
  for (int b = (int)MOTION_BRIGHTNESS; b >= 0; b -= 2) {
    mx.setBrightness((uint8_t)b);
    mx.show();
    delay(18);
  }
  mx.fillScreen(0);
  mx.show();
  mx.setBrightness(savedBrightness);
}

void stop() {
  Adafruit_NeoMatrix &mx = Display::Internal::matrix();
  mx.fillScreen(0);
  mx.show();
}

uint8_t motionStringToEnum(const String &motionStr) {
  if (motionStr == "DiagonalWave") {
    return 1;
  }
  return 0; // "Ripple" or default
}

String motionEnumToString(uint8_t animType) {
  if (animType == 1) {
    return "DiagonalWave";
  }
  return "Ripple";
}

uint16_t motionHueColor() {
  // リップル等の演出と同じ変換で MOTION_HUE の色を作る（彩度・明度は最大）
  return hsvToMatrixColor(MOTION_HUE, 255, 255);
}

} // namespace Animations
