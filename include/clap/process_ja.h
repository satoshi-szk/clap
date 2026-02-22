#pragma once

#include "events.h"
#include "audio-buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // 処理が失敗しました。出力バッファは破棄される必要があります。
   CLAP_PROCESS_ERROR = 0,

   // 処理が成功しました。処理を続行してください。
   CLAP_PROCESS_CONTINUE = 1,

   // 処理が成功しました。出力が静音でない場合は処理を続行してください。
   CLAP_PROCESS_CONTINUE_IF_NOT_QUIET = 2,

   // プラグインのテールに依存して、プラグインが処理を続行すべきかどうかを決定します。
   // clap_plugin_tailを参照してください
   CLAP_PROCESS_TAIL = 3,

   // 処理が成功しましたが、次のイベントまたはオーディオ入力の変化まで、
   // これ以上の処理は必要ありません。
   CLAP_PROCESS_SLEEP = 4,
};
typedef int32_t clap_process_status;

typedef struct clap_process {
   // 安定したサンプル時間カウンター。
   // このフィールドは、2つのプロセス呼び出し間のスリープ持続時間を
   // 計算するために使用できます。
   // この値はこのプラグインインスタンスに固有で、他のプラグインインスタンスが
   // 受信するものと関係がない場合があります。
   //
   // 利用できない場合は-1に設定し、そうでなければ値は0以上でなければならず、
   // 次のプロセス呼び出しでは少なくとも`frames_count`だけ増加させる必要があります。
   int64_t steady_time;

   // 処理するフレーム数
   uint32_t frames_count;

   // サンプル0での時刻情報
   // nullの場合、これはフリーランニングホストで、トランスポートイベントは提供されません
   const clap_event_transport_t *transport;

   // オーディオバッファ、clap_plugin_audio_ports->count()で指定されたのと
   // 同じ数である必要があります。
   // インデックスはclap_plugin_audio_ports->get()にマップされます。
   // 入力バッファとその内容は読み取り専用です。
   const clap_audio_buffer_t *audio_inputs;
   clap_audio_buffer_t       *audio_outputs;
   uint32_t                   audio_inputs_count;
   uint32_t                   audio_outputs_count;

   // 入力イベントリストは変更できません。
   // 入力読み取り専用イベントリスト。ホストはこれらをサンプル順にソートして配信します。
   const clap_input_events_t  *in_events;

   // 出力イベントリスト。プラグインはイベントを挿入する際に、サンプルソート順でイベントを挿入する必要があります
   const clap_output_events_t *out_events;
} clap_process_t;

#ifdef __cplusplus
}
#endif