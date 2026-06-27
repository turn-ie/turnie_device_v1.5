#pragma once
#include <Arduino.h>

// =============================================================
// Inbox
//   受信したメッセージを RAM 上のリングバッファに保存する。
//   古いものから順に INBOX_CAPACITY 件まで保持し、
//   それを超えると最も古いものから上書きする。
//   受信のたびに SD カードの JSON ファイル（/inbox.json）に上書き保存する。
// =============================================================
namespace Inbox {

    // 1 件のメッセージ
    struct Item {
        unsigned long receivedAt;  // 受信時刻（millis()）
        String        json;        // 受信した JSON 文字列
    };

    // SD から保存されたメッセージをロードする。
    bool init();

    // メッセージを追加する（内部で SD ログへの書き込みも行う）
    void append(const String& json);

    // 現在保存されているメッセージの件数
    int size();

    // index 番目のメッセージを取得する（0 = 最も古い、size()-1 = 最新）
    // 取得できた場合は true を返す
    bool get(int index, Item& out);

    // 全メッセージを削除する
    void clear();

}
