#include "Fluid.h"
#include "Config.h"
#include "Display.h"
#include "Content.h"
#include "Inbox.h"

#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <math.h>
#include "SensorQMI8658.hpp"

// =============================================================
// ピン（基板の QMI8658 は I2C SDA=11 / SCL=12。Game.ino と同一）
// =============================================================
#define IMU_SDA_PIN 11
#define IMU_SCL_PIN 12

// =============================================================
// マトリクス寸法（Config.h の 8x8）
// =============================================================
static const int MW = MATRIX_WIDTH;
static const int MH = MATRIX_HEIGHT;

// 待機画面には常に「自分が設定したコンテンツ」の粒を 1 つ表示する（index 0）。
//   そこから受信データ（Inbox）の粒を加えていく。
//   → Inbox が空なら粒は 1 つだけになる。
static const int OWN_PARTICLES = 1;

// =============================================================
// 粒の最大数（自分の粒 + Inbox 全件を出し切れる数）
// =============================================================
static const int MAX_PARTICLES = OWN_PARTICLES + INBOX_CAPACITY;

// =============================================================
// 物理パラメータ（Game.ino から移植）
// =============================================================
static const float GRAVITY        = 0.78f;  // 流体の加速の強さ（キビキビ＝大きめ）
static const float DAMPING        = 0.50f;  // 壁の跳ね返り（弱め＝壁際で暴れにくい）
static const float VELOC_FRICTION = 0.84f;  // 少し下げて加速が乗りやすく
static const float MAX_VELOC      = 2.90f;  // 直線移動の速度上限（引き上げ）
static const float ACC_SCALE      = 0.38f;  // IMU 値 → 重力スケール（感度アップ）

static const float DRAW_SMOOTH    = 0.92f;  // 表示位置の追従（大きいほどキビキビ＝尾が短い／小さいほど滑らかだが残像）

// 角（コーナー）対策
static const float CORNER_RANGE          = 2.25f;
static const float CORNER_ACC_MIN        = 0.20f;
static const float CORNER_FRICTION_MIN   = 0.54f;
static const float CORNER_DRAW_SMOOTH_MIN= 0.08f;
static const float CORNER_SPEED_MAX      = 0.55f; // 角に侵入した粒だけの速度上限

// 粒どうしの最小間隔（重なって 1 個に見えるのを防ぐ）
static const float MIN_DIST  = 1.05f;
static const float MIN_DIST2 = MIN_DIST * MIN_DIST;

// IMU ノイズ対策
//   ACC_LPF_ALPHA を上げると傾きへの反応が速くなる（＝キビキビ）。
//   下げると滑らかだが反応が鈍くなる。
static const float ACC_LPF_ALPHA = 0.26f;
static const float ACC_DEADBAND  = 0.10f;

// 軸の向き調整（実機で傾き方向が合わなければここで反転/入替する）
static const bool  IMU_SWAP_XY = false;
static const float IMU_SIGN_X  = -1.0f;
static const float IMU_SIGN_Y  = -1.0f;

// =============================================================
// 内部状態
// =============================================================
struct Vec2 { float x, y; };

static Vec2  pPos[MAX_PARTICLES];
static Vec2  pVel[MAX_PARTICLES];
static Vec2  pDraw[MAX_PARTICLES];
static uint8_t pColR[MAX_PARTICLES];
static uint8_t pColG[MAX_PARTICLES];
static uint8_t pColB[MAX_PARTICLES];
static float   pFade[MAX_PARTICLES];   // 0→1 のフェードイン係数（出現時に 0 から増える）

static int   g_count = 0;          // 現在画面に出ている粒の数

static SensorQMI8658 g_imu;
static bool   g_hasIMU = false;
static IMUdata g_accRaw;
static Vec2   g_accLPF = { 0, 0 };

// セルごとに重みと重み付き RGB を貯める
static uint32_t accW[MATRIX_PIXEL_COUNT];
static uint32_t accR[MATRIX_PIXEL_COUNT];
static uint32_t accG[MATRIX_PIXEL_COUNT];
static uint32_t accB[MATRIX_PIXEL_COUNT];

// =============================================================
// 小物
// =============================================================
static float sat01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }
static float lerpf(float a, float b, float t) { return a + (b - a) * t; }

static float cornerAmount(float x, float y) {
    float dx = fminf(x, (float)(MW - 1) - x);
    float dy = fminf(y, (float)(MH - 1) - y);
    float ex = sat01(1.0f - dx / CORNER_RANGE);
    float ey = sat01(1.0f - dy / CORNER_RANGE);
    return ex * ey;
}

static Vec2 cornerAwayDir(float x, float y) {
    Vec2 d;
    d.x = (x < (MW - 1) * 0.5f) ? 1.0f : -1.0f;
    d.y = (y < (MH - 1) * 0.5f) ? 1.0f : -1.0f;
    return d;
}

static float cornerBrakeAmount(Vec2 pos, Vec2 a) {
    float c = cornerAmount(pos.x, pos.y);
    if (c <= 0.0f) return 0.0f;
    Vec2 away = cornerAwayDir(pos.x, pos.y);
    float accMag = sqrtf(a.x * a.x + a.y * a.y);
    float still = sat01((0.050f - accMag) / 0.050f);
    float intoCorner = -(a.x * away.x + a.y * away.y);
    float pushing = sat01((intoCorner - 0.010f) / 0.090f);
    float brake = fmaxf(still, pushing);
    return c * brake;
}

static void clampParticle(int i) {
    if (pPos[i].x < 0.0f) pPos[i].x = 0.0f;
    if (pPos[i].x > MW - 1) pPos[i].x = MW - 1;
    if (pPos[i].y < 0.0f) pPos[i].y = 0.0f;
    if (pPos[i].y > MH - 1) pPos[i].y = MH - 1;
}

// =============================================================
// 代表色：受信データ 1 件 → 1 色
//   画像：画面全体で最も多く占める色（背景の黒は除外）
//   テキスト：白
//   取得できない場合（全て黒など）は MOTION_HUE の色にフォールバック
// =============================================================
static void hueToRgb(uint16_t hueDeg, uint8_t &r, uint8_t &g, uint8_t &b) {
    // 色相 0–360（度）を Adafruit の 16bit hue（0–65535）へ変換する
    uint16_t hue16 = (uint16_t)((uint32_t)hueDeg * 65536UL / 360UL);
    uint32_t c = Adafruit_NeoPixel::ColorHSV(hue16, 255, 255);
    r = (c >> 16) & 0xFF;
    g = (c >> 8) & 0xFF;
    b = c & 0xFF;
}

static void motionColorRgb(uint8_t &r, uint8_t &g, uint8_t &b) {
    hueToRgb(MOTION_HUE, r, g, b);
}

// 画像の代表色を「最も多く占める色」で決めるための設定
//   似た色をまとめるため各チャンネルを COLOR_LEVELS 段階に量子化して集計する。
static const int COLOR_LEVELS  = 8;  // チャンネルあたりの量子化段階（>>5）
static const int COLOR_BUCKETS = COLOR_LEVELS * COLOR_LEVELS * COLOR_LEVELS;
static const int BG_BLACK_MAX  = 16; // max(r,g,b) がこの値未満なら背景の黒として無視

static int colorBucket(uint8_t r, uint8_t g, uint8_t b) {
    return ((r >> 5) * COLOR_LEVELS + (g >> 5)) * COLOR_LEVELS + (b >> 5);
}

static void representativeColor(const Content::Data &d, uint8_t &r, uint8_t &g, uint8_t &b) {
    if (d.kind == Content::Kind::Image) {
        // 画面全体で最も多く占める色を代表色にする。
        //   背景の黒（ほぼ消灯）は集計から除外する。
        uint8_t count[COLOR_BUCKETS];
        memset(count, 0, sizeof(count));

        int bestBucket = -1;
        int bestCount  = 0;
        for (int p = 0; p < MATRIX_PIXEL_COUNT; ++p) {
            uint8_t pr = d.rgb[p * 3];
            uint8_t pg = d.rgb[p * 3 + 1];
            uint8_t pb = d.rgb[p * 3 + 2];
            if (max(pr, max(pg, pb)) < BG_BLACK_MAX) continue; // 背景の黒は無視
            int bkt = colorBucket(pr, pg, pb);
            if (++count[bkt] > bestCount) {
                bestCount  = count[bkt];
                bestBucket = bkt;
            }
        }

        if (bestBucket < 0) {        // 全て黒など → モーション色へフォールバック
            motionColorRgb(r, g, b);
            return;
        }

        // 勝った色グループに属する実ピクセルの平均色を返す（量子化誤差をならす）
        uint32_t sr = 0, sg = 0, sb = 0;
        int n = 0;
        for (int p = 0; p < MATRIX_PIXEL_COUNT; ++p) {
            uint8_t pr = d.rgb[p * 3];
            uint8_t pg = d.rgb[p * 3 + 1];
            uint8_t pb = d.rgb[p * 3 + 2];
            if (max(pr, max(pg, pb)) < BG_BLACK_MAX) continue;
            if (colorBucket(pr, pg, pb) == bestBucket) {
                sr += pr; sg += pg; sb += pb; ++n;
            }
        }
        r = (uint8_t)(sr / n);
        g = (uint8_t)(sg / n);
        b = (uint8_t)(sb / n);
        return;
    }

    if (d.kind == Content::Kind::Text) {
        // テキストは白
        r = 255; g = 255; b = 255;
        return;
    }

    motionColorRgb(r, g, b);
}

// フェードインの速さ（1フレームあたりの増分。大きいほど速く出現）
//   待機ループは約16ms周期なので、0.04 ≒ 約0.4秒で出現。
static const float FADE_RATE = 0.04f;

// 待機（流体）へ戻った瞬間の「画面全体」フェードイン。
//   tick() が FLUID_RESUME_GAP_MS 以上呼ばれていなかった＝その間プレビューや
//   受信表示・ripple 等で何かを表示していた、とみなして毎回フェードインさせる。
//   これで「通常状態へ戻るときは必ずフェードイン」を全パス共通で実現する。
static const unsigned long FLUID_RESUME_GAP_MS = 80;    // この時間以上空いたら再フェード
static const float         SCREEN_FADE_RATE    = 0.06f; // 1フレーム増分（約0.27秒で全開）
static unsigned long       g_lastTickMs        = 0;
static float               g_screenFade        = 1.0f;  // 0→1。1=通常（フェード無し）

// 新しい粒をランダムな位置に置き、フェードイン（pFade=0 から）で出現させる。
static void spawnParticle(int i) {
    pPos[i].x = random(0, MW);
    pPos[i].y = random(0, MH);
    pVel[i].x = 0.0f;
    pVel[i].y = 0.0f;
    pDraw[i] = pPos[i];
    pFade[i] = 0.0f;   // フェードイン開始
}

// =============================================================
// Public
// =============================================================
namespace Fluid {

void init() {
    randomSeed(esp_random());

    // IMU 初期化（無ければ無重力でふわふわ漂う動作にフォールバック）
    Wire.begin(IMU_SDA_PIN, IMU_SCL_PIN);
    Wire.setClock(400000);
    if (g_imu.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IMU_SDA_PIN, IMU_SCL_PIN)) {
        g_imu.configAccelerometer(
            SensorQMI8658::ACC_RANGE_4G,
            SensorQMI8658::ACC_ODR_250Hz,
            SensorQMI8658::LPF_MODE_0);
        g_imu.enableAccelerometer();
        g_hasIMU = true;
        Serial.println("  [Fluid] QMI8658 ready");
    } else {
        g_hasIMU = false;
        Serial.println("  [Fluid] QMI8658 not found -> drift mode");
    }

    // 表示の明るさはユーザー設定（MOTION_BRIGHTNESS）に合わせる
    Display::Internal::matrix().setBrightness(MOTION_BRIGHTNESS);

    g_count = 0;
    syncFromInbox();
}

void syncFromInbox() {
    int received = Inbox::size();
    int target = OWN_PARTICLES + received;   // 自分の粒(1) + 受信数
    if (target > MAX_PARTICLES) target = MAX_PARTICLES;

    // 代表色を粒へ割り当てる。
    //   index 0           = 自分が設定したコンテンツ（常に表示）
    //   index 1..received = 受信データ（古い順。増えた分が新着）
    //   こうすると新着メッセージが「新しく注ぎ込まれる粒」に対応する。
    for (int i = 0; i < target; ++i) {
        uint8_t r, g, b;
        bool assigned = false;
        if (i < OWN_PARTICLES) {
            // 自分のコンテンツの代表色（未設定なら representativeColor が
            // モーション色へフォールバックする）
            representativeColor(Content::own(), r, g, b);
            assigned = true;
        } else {
            int rxIndex = i - OWN_PARTICLES;
            Inbox::Item item;
            Content::Data d;
            if (Inbox::get(rxIndex, item) && Content::parse(item.json, d)) {
                representativeColor(d, r, g, b);
                assigned = true;
            }
        }
        if (!assigned) {
            motionColorRgb(r, g, b);   // パース失敗時のフォールバック
        }
        pColR[i] = r; pColG[i] = g; pColB[i] = b;
    }

    // 減った場合は即座に詰める。
    //   増えた分は新しい粒を spawn（フェードインで出現）させる。
    if (target < g_count) {
        g_count = target;
    } else {
        for (int i = g_count; i < target; ++i) spawnParticle(i);
        g_count = target;
    }
}

// ---- 物理更新 ----
static void updateParticles(Vec2 a) {
    a.x *= ACC_SCALE;
    a.y *= ACC_SCALE;

    bool almostStill = fabsf(a.x) + fabsf(a.y) < 0.012f;

    for (int i = 0; i < g_count; ++i) {
        float brake = cornerBrakeAmount(pPos[i], a);
        float localAcc = lerpf(1.0f, CORNER_ACC_MIN, brake);
        float localFriction = lerpf(VELOC_FRICTION, CORNER_FRICTION_MIN, brake);

        pVel[i].x = pVel[i].x * localFriction + a.x * GRAVITY * localAcc;
        pVel[i].y = pVel[i].y * localFriction + a.y * GRAVITY * localAcc;

        if (almostStill && fabsf(pVel[i].x) < 0.012f) pVel[i].x = 0.0f;
        if (almostStill && fabsf(pVel[i].y) < 0.012f) pVel[i].y = 0.0f;
        if (brake > 0.45f && fabsf(pVel[i].x) < 0.040f) pVel[i].x = 0.0f;
        if (brake > 0.45f && fabsf(pVel[i].y) < 0.040f) pVel[i].y = 0.0f;

        pVel[i].x = constrain(pVel[i].x, -MAX_VELOC, MAX_VELOC);
        pVel[i].y = constrain(pVel[i].y, -MAX_VELOC, MAX_VELOC);

        // 角に侵入した粒だけ速度を強制クランプ
        float cornerC = cornerAmount(pPos[i].x, pPos[i].y);
        if (cornerC > 0.001f) {
            float cap = lerpf(MAX_VELOC, CORNER_SPEED_MAX, cornerC);
            float sp2 = pVel[i].x * pVel[i].x + pVel[i].y * pVel[i].y;
            if (sp2 > cap * cap) {
                float s = cap / sqrtf(sp2);
                pVel[i].x *= s;
                pVel[i].y *= s;
            }
        }

        float nx = pPos[i].x + pVel[i].x;
        float ny = pPos[i].y + pVel[i].y;

        if (nx < 0.0f) {
            nx = 0.0f; pVel[i].x = fabsf(pVel[i].x) * DAMPING;
            if (fabsf(pVel[i].x) < 0.025f) pVel[i].x = 0.0f;
        } else if (nx > MW - 1) {
            nx = MW - 1; pVel[i].x = -fabsf(pVel[i].x) * DAMPING;
            if (fabsf(pVel[i].x) < 0.025f) pVel[i].x = 0.0f;
        }
        if (ny < 0.0f) {
            ny = 0.0f; pVel[i].y = fabsf(pVel[i].y) * DAMPING;
            if (fabsf(pVel[i].y) < 0.025f) pVel[i].y = 0.0f;
        } else if (ny > MH - 1) {
            ny = MH - 1; pVel[i].y = -fabsf(pVel[i].y) * DAMPING;
            if (fabsf(pVel[i].y) < 0.025f) pVel[i].y = 0.0f;
        }

        pPos[i].x = nx;
        pPos[i].y = ny;
    }

    // 粒どうしの分離（重なって 1 個に見えるのを防ぐ）
    for (int pass = 0; pass < 4; ++pass) {
        for (int i = 0; i < g_count; ++i) {
            for (int j = i + 1; j < g_count; ++j) {
                float dx = pPos[j].x - pPos[i].x;
                float dy = pPos[j].y - pPos[i].y;
                float dist2 = dx * dx + dy * dy;
                if (dist2 < MIN_DIST2) {
                    float dist = sqrtf(dist2);
                    float ang = (dist > 0.001f) ? atan2f(dy, dx) : (float)(i + j + 1);
                    float push = (MIN_DIST - dist) * 0.5f;
                    float repX = cosf(ang) * push;
                    float repY = sinf(ang) * push;
                    pPos[i].x -= repX; pPos[i].y -= repY;
                    pPos[j].x += repX; pPos[j].y += repY;
                    pVel[i].x *= 0.88f; pVel[i].y *= 0.88f;
                    pVel[j].x *= 0.88f; pVel[j].y *= 0.88f;
                    clampParticle(i);
                    clampParticle(j);
                }
            }
        }
    }
}

// バイリニアの「にじみ（広がり）」を抑える。
//   小数位置を最寄りセル側へ寄せて、隣セルへ漏れる明るさを減らす。
//   1.0 で従来のリニア、大きいほど 1LED に集中する（補間自体は残るので
//   サブピクセルの滑らかさは保たれる）。
static const float SPLAT_SHARPNESS = 2.4f;
static float splatSharpen(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    if (t < 0.5f)  return 0.5f * powf(2.0f * t, SPLAT_SHARPNESS);
    return 1.0f - 0.5f * powf(2.0f * (1.0f - t), SPLAT_SHARPNESS);
}

// ---- 周囲4セルへバイリニア合成 ----
static void splat(int i) {
    float fx = constrain(pDraw[i].x, 0.0f, (float)(MW - 1));
    float fy = constrain(pDraw[i].y, 0.0f, (float)(MH - 1));
    int x0 = (int)floorf(fx), y0 = (int)floorf(fy);
    float tx = fx - x0, ty = fy - y0;
    int x1 = x0 + 1, y1 = y0 + 1;
    if (x1 >= MW) { x1 = x0; tx = 0.0f; }
    if (y1 >= MH) { y1 = y0; ty = 0.0f; }

    // にじみ低減：小数位置を最寄りセルへ寄せてから重みを計算する
    tx = splatSharpen(tx);
    ty = splatSharpen(ty);

    float f = pFade[i];   // フェードイン係数（0→1）で明るさを上げていく
    uint16_t w[4] = {
        (uint16_t)((1 - tx) * (1 - ty) * 255.0f * f),
        (uint16_t)(tx * (1 - ty) * 255.0f * f),
        (uint16_t)((1 - tx) * ty * 255.0f * f),
        (uint16_t)(tx * ty * 255.0f * f)
    };
    int cx[4] = { x0, x1, x0, x1 };
    int cy[4] = { y0, y0, y1, y1 };

    for (int k = 0; k < 4; ++k) {
        if (cx[k] < 0 || cx[k] >= MW || cy[k] < 0 || cy[k] >= MH) continue;
        int idx = cy[k] * MW + cx[k];
        accW[idx] += w[k];
        accR[idx] += (uint32_t)pColR[i] * w[k];
        accG[idx] += (uint32_t)pColG[i] * w[k];
        accB[idx] += (uint32_t)pColB[i] * w[k];
    }
}

static void render() {
    Adafruit_NeoMatrix &mx = Display::Internal::matrix();

    memset(accW, 0, sizeof(accW));
    memset(accR, 0, sizeof(accR));
    memset(accG, 0, sizeof(accG));
    memset(accB, 0, sizeof(accB));

    for (int i = 0; i < g_count; ++i) {
        // 表示位置をなめらかに追従させる
        float brake = cornerBrakeAmount(pPos[i], (Vec2){0, 0});
        float sm = lerpf(DRAW_SMOOTH, CORNER_DRAW_SMOOTH_MIN, brake);
        pDraw[i].x += (pPos[i].x - pDraw[i].x) * sm;
        pDraw[i].y += (pPos[i].y - pDraw[i].y) * sm;
        splat(i);
    }

    for (int y = 0; y < MH; ++y) {
        for (int x = 0; x < MW; ++x) {
            int idx = y * MW + x;
            uint32_t w = accW[idx];
            uint8_t r = 0, g = 0, b = 0;
            if (w > 0) {
                uint32_t d = (w > 1400) ? 1400 : w;
                uint8_t v;
                if (d < 255)      v = 28 + (d * 120) / 255;
                else if (d < 510) v = 148 + ((d - 255) * 62) / 255;
                else              v = 220;
                // 待機へ戻った直後は画面全体をフェードインさせる
                if (g_screenFade < 1.0f) v = (uint8_t)(v * g_screenFade);
                uint8_t baseR = accR[idx] / w;
                uint8_t baseG = accG[idx] / w;
                uint8_t baseB = accB[idx] / w;
                r = (uint16_t)baseR * v / 255;
                g = (uint16_t)baseG * v / 255;
                b = (uint16_t)baseB * v / 255;
            }
            mx.drawPixel(x, y, Display::Internal::color(r, g, b));
        }
    }
    mx.show();
}

void tick() {
    // 画面全体のフェードイン：待機へ戻った最初のフレーム（前回 tick から時間が
    // 空いている＝何かを表示していた）なら 0 から再スタートし、毎フレーム上げていく。
    unsigned long now = millis();
    if (now - g_lastTickMs > FLUID_RESUME_GAP_MS) {
        g_screenFade = 0.0f;
    }
    g_lastTickMs = now;
    if (g_screenFade < 1.0f) {
        g_screenFade += SCREEN_FADE_RATE;
        if (g_screenFade > 1.0f) g_screenFade = 1.0f;
    }

    // フェードイン：出現直後の粒を毎フレーム少しずつ濃くする。
    for (int i = 0; i < g_count; ++i) {
        if (pFade[i] < 1.0f) {
            pFade[i] += FADE_RATE;
            if (pFade[i] > 1.0f) pFade[i] = 1.0f;
        }
    }

    // IMU 読み取り（LPF + デッドバンド）
    Vec2 a = { 0, 0 };
    if (g_hasIMU && g_imu.getDataReady()) {
        g_imu.getAccelerometer(g_accRaw.x, g_accRaw.y, g_accRaw.z);
        g_accLPF.x += ACC_LPF_ALPHA * (g_accRaw.x - g_accLPF.x);
        g_accLPF.y += ACC_LPF_ALPHA * (g_accRaw.y - g_accLPF.y);

        float gx = (fabsf(g_accLPF.x) < ACC_DEADBAND) ? 0.0f : g_accLPF.x;
        float gy = (fabsf(g_accLPF.y) < ACC_DEADBAND) ? 0.0f : g_accLPF.y;

        if (IMU_SWAP_XY) { float t = gx; gx = gy; gy = t; }
        a.x = gx * IMU_SIGN_X;
        a.y = gy * IMU_SIGN_Y;
    }

    updateParticles(a);
    render();
}

} // namespace Fluid
