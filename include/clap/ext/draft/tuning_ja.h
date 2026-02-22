#pragma once

#include "../../plugin.h"
#include "../../events.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_TUNING[] = "clap.tuning/2";

#ifdef __cplusplus
extern "C" {
#endif

// イベントスペースを知るには、clap_host_event_registry->query(host, CLAP_EXT_TUNING, &space_id)を使用してください。
//
// このイベントは、指定されたポート/チャンネルで使用されるチューニングを定義します。
typedef struct clap_event_tuning {
   clap_event_header_t header;

   int16_t port_index; // -1 グローバル
   int16_t channel;    // 0..15, -1 グローバル
   clap_id tunning_id;
} clap_event_tuning_t;

typedef struct clap_tuning_info {
   clap_id tuning_id;
   char    name[CLAP_NAME_SIZE];
   bool    is_dynamic; // 値が時間と共に変動する可能性がある場合はtrue
} clap_tuning_info_t;

typedef struct clap_plugin_tuning {
   // チューニングがプールに追加または削除されたときに呼び出されます。
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_plugin_t *plugin);
} clap_plugin_tuning_t;

// この拡張は、プラグインに動的チューニングテーブルを提供します。
typedef struct clap_host_tuning {
   // A4=440Hzの平均律に対する相対チューニングをセミトーン単位で取得します。
   // プラグインは、*低*周波数変調に適した頻度でチューニングをクエリすることができます。
   //
   // tuning_idが見つからないか、CLAP_INVALID_IDと等しい場合、
   // この関数は適切な値を優雅に返すべきです。
   //
   // sample_offsetは、現在の処理ブロックの開始からのサンプルオフセットです。
   //
   // この関数を呼び出す前に、should_play(...)をチェックする必要があります。
   //
   // [audio-thread & in-process]
   double(CLAP_ABI *get_relative)(const clap_host_t *host,
                                  clap_id            tuning_id,
                                  int32_t            channel,
                                  int32_t            key,
                                  uint32_t           sample_offset);

   // ノートを演奏すべき場合はtrueを返します。
   // [audio-thread & in-process]
   bool(CLAP_ABI *should_play)(const clap_host_t *host,
                               clap_id            tuning_id,
                               int32_t            channel,
                               int32_t            key);

   // プール内のチューニング数を返します。
   // [main-thread]
   uint32_t(CLAP_ABI *get_tuning_count)(const clap_host_t *host);

   // チューニングに関する情報を取得します
   // 成功時にtrueを返し、結果をinfoに格納します。
   // [main-thread]
   bool(CLAP_ABI *get_info)(const clap_host_t  *host,
                            uint32_t            tuning_index,
                            clap_tuning_info_t *info);
} clap_host_tuning_t;

#ifdef __cplusplus
}
#endif