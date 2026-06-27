#pragma once
#include <Arduino.h>

// =============================================================
// Animations
//   8x8 マトリクス用のノンブロッキング演出アニメーション。
//   Display::Internal::matrix() で直接ピクセルを操作する。
//   各アニメーションは start*() で開始し、loop() から
//   tick() を呼ぶことでフレームを進める。
// =============================================================
namespace Animations {

    // アニメーション中なら true を返す（tick() が false を返したら終了）
    bool isRunning();

    // loop() から毎回呼ぶ。アニメーション中なら 1 フレーム進める。
    // 終了したフレームで false を返す。
    bool tick();

    // MOTION_ANIM の値に従ってアニメーションを選択して開始する
    //   0 = Ripple / 1 = DiagonalWave
    void startMotion();

    // 受信演出：中心から外へ広がるリップル（ブロッキング）
    void startRipple();

    // すれ違い演出：左右端から中央へ収束する波（ブロッキング）
    void startDiagonalWave();

    // モーション名文字列を内部のモーションID (0=Ripple, 1=DiagonalWave) に変換する
    uint8_t motionStringToEnum(const String& motionStr);

    // 内部のモーションIDをモーション名文字列に変換する
    String motionEnumToString(uint8_t animType);

    // 現在の MOTION_HUE をマトリクス用の 16bit カラーで返す（リップル等と同じ色味）
    uint16_t motionHueColor();

    // 再生中のアニメーションを即停止し、画面をクリアする
    void stop();

}
