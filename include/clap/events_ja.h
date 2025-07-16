#pragma once

#include "private/std.h"
#include "fixedpoint.h"
#include "id.h"

#ifdef __cplusplus
extern "C" {
#endif

// イベントヘッダー
// すべてのclapイベントは、イベント全体のサイズとその型と空間
//（型のネームスペース）を決定するイベントヘッダーで始まります。
// clap_eventオブジェクトは、ヘッダーの先頭から`size`バイトの
// memcpyでコピーできる連続したメモリ領域です。そのため、
// 内部ポインターやその他の非値型を持つclapイベントを設計する際は、
// それらのメンバーの寿命を考慮して非常に注意してください。
typedef struct clap_event_header {
   uint32_t size;     // このヘッダーを含むイベントサイズ、例: sizeof (clap_event_note)
   uint32_t time;     // このイベントのバッファ内のサンプルオフセット
   uint16_t space_id; // イベント空間、clap_host_event_registryを参照
   uint16_t type;     // イベント型
   uint32_t flags;    // clap_event_flagsを参照
} clap_event_header_t;

// clapコアイベント空間
static const CLAP_CONSTEXPR uint16_t CLAP_CORE_EVENT_SPACE_ID = 0;

enum clap_event_flags {
   // ユーザーが物理的なノブを回したり、物理的なキーを演奏したりするなど、
   // ライブユーザーイベントを示します。
   CLAP_EVENT_IS_LIVE = 1 << 0,

   // イベントが記録されるべきではないことを示します。
   // 例えば、MIDI CCのためにパラメータが変更される場合に有用です。
   // ホストがMIDI CCオートメーションとパラメータオートメーションの
   // 両方を記録すると競合が発生するためです。
   CLAP_EVENT_DONT_RECORD = 1 << 1,
};

// 以下のいくつかのイベントは重複しており、ノートオンは以下で表現できます:
// - CLAP_EVENT_NOTE_ON
// - CLAP_EVENT_MIDI
// - CLAP_EVENT_MIDI2
//
// ノートイベントを送信する好ましい方法は CLAP_EVENT_NOTE_* を使用することです。
//
// 同じイベントを2回送信してはいけません: 同じノートオンを
// CLAP_EVENT_NOTE_ON と CLAP_EVENT_MIDI の両方でエンコードして送信することは禁止されています。
//
// プラグインは、生のmidiまたはmidi2としてエンコードされたノートイベントを
// 処理できることが推奨されるか、clap_plugin_event_filterを実装して
// 生のmidiおよびmidi2イベントを拒否してください。
enum {
   // NOTE_ONとNOTE_OFFは、それぞれキーが押されたイベントとキーが離されたイベントを表します。
   // ベロシティが0のNOTE_ONは有効であり、NOTE_OFFとして解釈されるべきではありません。
   //
   // NOTE_CHOKEは、ドラムマシンでクローズドハイハットがオープンハイハットを
   // チョークするように、ボイスをチョークすることを意味します。このイベントは
   // ホストからプラグインに送信できます。以下に2つの使用例があります:
   // - プラグインがBitwig StudioのドラムマシンのドラムパッドWOの中にあり、
   //   このパッドが別のパッドによってチョークされる場合
   // - ユーザーがトランスポートのDAWの停止ボタンをダブルクリックして、
   //   すべてのトラックの音を停止する場合
   //
   // NOTE_ENDはプラグインからホストに送信されます。port、channel、key、note_idは
   // ホストがNOTE_ONイベントで与えたものです。言い換えると、このイベントは
   // プラグインのノート入力ポートに対してマッチされます。
   // NOTE_ENDは、ホストがプラグインのボイスライフタイムをマッチするのに有用です。
   //
   // ポリフォニックモジュレーションを使用する場合、ホストはポリフォニック
   // モジュレーターのボイスを割り当てて解放する必要があります。しかし、
   // プラグインだけがホストがボイスを終了すべき時を効果的に知っています。
   // NOTE_ENDは、非侵入的で協力的な方法でその問題を解決します。
   //
   // CLAPは、ホストが指定されたポート、チャンネル、キーのNOTE_ONイベントに対して
   // 一意のボイスを割り当てることを想定しています。このボイスは、プラグインが
   // NOTE_ENDイベントを送信してホストに終了を指示するまで実行されます。
   //
   // 以下のシーケンスを考えてみてください:
   // - process()
   //    Host->Plugin NoteOn(port:0, channel:0, key:16, time:t0)
   //    Host->Plugin NoteOn(port:0, channel:0, key:64, time:t0)
   //    Host->Plugin NoteOff(port:0, channel:0, key:16, t1)
   //    Host->Plugin NoteOff(port:0, channel:0, key:64, t1)
   //    # t2で、両方のノートが終了した
   //    Host->Plugin NoteOn(port:0, channel:0, key:64, t3)
   //    # ここで、プラグインはすべてのフレームの処理を完了し、ホストに
   //    # キー16のボイスを終了するよう指示しますが、64は終了しません。
   //    # なぜなら、t3でノートが開始されたからです。
   //    Plugin->Host NoteEnd(port:0, channel:0, key:16, time:ignored)
   //
   // これら4つのイベントはclap_event_noteを使用します。
   CLAP_EVENT_NOTE_ON = 0,
   CLAP_EVENT_NOTE_OFF = 1,
   CLAP_EVENT_NOTE_CHOKE = 2,
   CLAP_EVENT_NOTE_END = 3,

   // ノートエクスプレッションを表します。
   // clap_event_note_expressionを使用します。
   CLAP_EVENT_NOTE_EXPRESSION = 4,

   // PARAM_VALUEはパラメータの値を設定します; clap_event_param_valueを使用します。
   // PARAM_MODはパラメータのモジュレーション量を設定します; clap_event_param_modを使用します。
   //
   // 聞こえる値は: param_value + param_mod です。
   //
   // 同時のグローバル値/モジュレーション対ポリフォニックな値の場合、
   // ボイスはポリフォニックなもののみを使用し、ポリフォニックモジュレーション量は
   // すでにモノフォニック信号を含んでいます。
   CLAP_EVENT_PARAM_VALUE = 5,
   CLAP_EVENT_PARAM_MOD = 6,

   // ユーザーがノブの調整を開始または終了したことを示します。
   // パラメータ変更をジェスチャーイベントでラップすることは必須ではありませんが、
   // オートメーションの記録またはオートメーション再生のオーバーライド時に
   // ユーザーエクスペリエンスが大幅に向上します。
   // clap_event_param_gestureを使用します。
   CLAP_EVENT_PARAM_GESTURE_BEGIN = 7,
   CLAP_EVENT_PARAM_GESTURE_END = 8,

   CLAP_EVENT_TRANSPORT = 9,   // トランスポート情報を更新; clap_event_transport
   CLAP_EVENT_MIDI = 10,       // 生のmidiイベント; clap_event_midi
   CLAP_EVENT_MIDI_SYSEX = 11, // 生のmidi sysexイベント; clap_event_midi_sysex
   CLAP_EVENT_MIDI2 = 12,      // 生のmidi 2イベント; clap_event_midi2
};

// ノートオン、オフ、エンド、チョークイベント。
//
// Clapは4値タプル(port, channel, key, note_id)を使用してノートとボイスをアドレスします。
// ノートオン/オフ/エンド/チョークイベントとパラメータモジュレーションメッセージは、
// これらの値が設定されて配信されます。
//
// ノートとボイスアドレスの値は、指定されている場合は>= 0であるか、
// ワイルドカードを示すために-1です。ワイルドカードは、タプルのその部分に
// 任意の値を持つボイスがメッセージにマッチすることを意味します。
//
// 例えば、(PCKN)が(0, 3, -1, -1)の場合、ポート0のチャンネル3のすべてのボイスにマッチします。
// そして、PCKNが(-1, 0, 60, -1)の場合、ポートやノートIDに関係なく、
// すべてのチャンネル0キー60ボイスにマッチします。
//
// 特に、ノートオン・ノートオフペアの場合、およびボイススタッキングや
// ポリフォニックモジュレーションがない場合、ホストはノートオン時にのみ
// ノートIDを発行することを選択できます。そのため、以下のようなメッセージストリームを
// 見る可能性があります:
//
// CLAP_EVENT_NOTE_ON  [0,0,60,184]
// CLAP_EVENT_NOTE_OFF [0,0,60,-1]
//
// そして、ホストは最初のボイスがリリースされることを期待します。
// よく構築されたプラグインは、タプル全体を使用してボイスとノートを検索します。
//
// ノートオンイベントの場合:
// - port、channel、keyは>= 0の値で指定される必要があります
// - port、channel、またはkeyに'-1'を持つノートオンイベントは無効であり、
//   プラグインまたはホストによって拒否または無視される可能性があります。
// - ノートIDをサポートしないホストは、ノートIDを-1に設定する必要があります。
//
// ノートチョークまたはエンドイベントの場合:
// - ベロシティは無視されます。
// - keyとchannelはアクティブなノートをマッチするために使用されます
// - note_idはホストによってオプションで提供されます
typedef struct clap_event_note {
   clap_event_header_t header;

   int32_t note_id; // ホストが提供するノートID >= 0、または未指定/ワイルドカードの場合は-1
   int16_t port_index; // ext/note-portsからのポートインデックス; ワイルドカードの場合は-1
   int16_t channel;  // 0..15、MIDI1チャンネル番号と同じ、ワイルドカードの場合は-1
   int16_t key;      // 0..127、MIDI1キー番号と同じ（60==ミドルC）、ワイルドカードの場合は-1
   double  velocity; // 0..1
} clap_event_note_t;

// ノートエクスプレッションは、上記で説明したワイルドカードルールを使用して
// ボイスをターゲットとする、ボイスの適切に名付けられた修正です。
// ノートエクスプレッションはサンプル精度のイベントとして配信され、
// 受信時にサンプルで適用される必要があります。
//
// ノートエクスプレッションは値の記述であり、累積的ではありません。
// 0のPANイベントの後に1、その後に0.5が続く場合、ハードレフト、ハードライト、
// センターにパンします。これらは、ノートエクスプレッション以外のボイスデフォルトから
// のオフセットとして意図されています。ノートエクスプレッションがない状態で-20dbの
// ボリュームを持つボイスが+4dbのノートエクスプレッションを受信した場合、
// ボイスは-16dbに移動します。
//
// NOTE_ONイベントと同じサンプルでノートエクスプレッションを受信するプラグインは、
// そのエクスプレッションをすべての生成されたサンプルに適用する必要があります。
// NOTE_ONイベント後にノートエクスプレッションを受信するプラグインは、
// デフォルト値でボイスを開始し、受信時にノートエクスプレッションを適用する必要があります。
// プラグインは、ノートエクスプレッションストリームを滑らかにする選択をしても構いません。
enum {
   // 0 < x <= 4で、plain = 20 * log(x)
   CLAP_NOTE_EXPRESSION_VOLUME = 0,

   // パン、0は左、0.5はセンター、1は右
   CLAP_NOTE_EXPRESSION_PAN = 1,

   // -120から+120までの半音での相対チューニング。半音は等音律で
   // ダブルです; 結果のノートは`100 * evt->value`セントでリチューンされます。
   CLAP_NOTE_EXPRESSION_TUNING = 2,

   // 0..1
   CLAP_NOTE_EXPRESSION_VIBRATO = 3,
   CLAP_NOTE_EXPRESSION_EXPRESSION = 4,
   CLAP_NOTE_EXPRESSION_BRIGHTNESS = 5,
   CLAP_NOTE_EXPRESSION_PRESSURE = 6,
};
typedef int32_t clap_note_expression;

typedef struct clap_event_note_expression {
   clap_event_header_t header;

   clap_note_expression expression_id;

   // 特定のnote_id、port、key、channelをターゲットとし、
   // -1は上記のワイルドカードディスカッションに従ってワイルドカードを意味します
   int32_t note_id;
   int16_t port_index;
   int16_t channel;
   int16_t key;

   double value; // 範囲については expression を参照
} clap_event_note_expression_t;

typedef struct clap_event_param_value {
   clap_event_header_t header;

   // ターゲットパラメータ
   clap_id param_id; // @ref clap_param_info.id
   void   *cookie;   // @ref clap_param_info.cookie

   // 特定のnote_id、port、key、channelをターゲットとし、
   // -1は上記のワイルドカードディスカッションに従ってワイルドカードを意味します
   int32_t note_id;
   int16_t port_index;
   int16_t channel;
   int16_t key;

   double value;
} clap_event_param_value_t;

typedef struct clap_event_param_mod {
   clap_event_header_t header;

   // ターゲットパラメータ
   clap_id param_id; // @ref clap_param_info.id
   void   *cookie;   // @ref clap_param_info.cookie

   // 特定のnote_id、port、key、channelをターゲットとし、
   // -1は上記のワイルドカードディスカッションに従ってワイルドカードを意味します
   int32_t note_id;
   int16_t port_index;
   int16_t channel;
   int16_t key;

   double amount; // モジュレーション量
} clap_event_param_mod_t;

typedef struct clap_event_param_gesture {
   clap_event_header_t header;

   // ターゲットパラメータ
   clap_id param_id; // @ref clap_param_info.id
} clap_event_param_gesture_t;

enum clap_transport_flags {
   CLAP_TRANSPORT_HAS_TEMPO = 1 << 0,
   CLAP_TRANSPORT_HAS_BEATS_TIMELINE = 1 << 1,
   CLAP_TRANSPORT_HAS_SECONDS_TIMELINE = 1 << 2,
   CLAP_TRANSPORT_HAS_TIME_SIGNATURE = 1 << 3,
   CLAP_TRANSPORT_IS_PLAYING = 1 << 4,
   CLAP_TRANSPORT_IS_RECORDING = 1 << 5,
   CLAP_TRANSPORT_IS_LOOP_ACTIVE = 1 << 6,
   CLAP_TRANSPORT_IS_WITHIN_PRE_ROLL = 1 << 7,
};

// clap_event_transportは、ホストからプラグインに楽曲位置、テンポ、
// および類似情報を提供します。ホストがこれらの値を伝達する方法は2つあります。
// 各処理ブロックに送信される`clap_process`構造体において、ホストは
// ブロックの開始時に利用可能な情報を示すトランスポート構造体を提供する場合があります。
// ホストがサンプル精度のテンポまたはトランスポート変更を提供する場合、
// 新しいイベントを配信することにより、その後のブロック間トランスポート更新も提供できます。
typedef struct clap_event_transport {
   clap_event_header_t header;

   uint32_t flags; // clap_transport_flagsを参照

   clap_beattime song_pos_beats;   // ビート単位の位置
   clap_sectime  song_pos_seconds; // 秒単位の位置

   double tempo;     // bpm単位
   double tempo_inc; // 各サンプルおよび次のタイム情報イベントまでの
                     // テンポ増分

   clap_beattime loop_start_beats;
   clap_beattime loop_end_beats;
   clap_sectime  loop_start_seconds;
   clap_sectime  loop_end_seconds;

   clap_beattime bar_start;  // 現在の小節の開始位置
   int32_t       bar_number; // 楽曲位置0の小節は番号0を持つ

   uint16_t tsig_num;   // 拍子記号の分子
   uint16_t tsig_denom; // 拍子記号の分母
} clap_event_transport_t;

typedef struct clap_event_midi {
   clap_event_header_t header;

   uint16_t port_index;
   uint8_t  data[3];
} clap_event_midi_t;

// clap_event_midi_sysexは、sysex内容バッファへのポインターを含みます。
// このバッファの寿命は（ホスト->プラグインから）イベントが配信される
// プロセス呼び出しのみ、または（プラグイン->ホストから）try_push呼び出しの
// 期間のみです。
//
// `clap_output_events.try_push`はホストにイベントのコピーを作成することを
// 要求するため、プラグインからsysexメッセージを受信するホスト実装者は、
// イベント（ヘッダー、サイズなど）をコピーするだけでなく、
// sysexポインターの内容をホスト所有のメモリにmemcpyし、
// データポインターだけをコピーしないよう注意する必要があります。
//
// 同様に、単一のプロセス呼び出しの寿命を超えてsysexを保持するプラグインは、
// sysexバッファをプラグイン所有のメモリにコピーする必要があります。
//
// その結果、sysexバッファによって指されるデータ構造は、
// `size`バイトの`memcpy`で連続してコピー可能でなければなりません。
typedef struct clap_event_midi_sysex {
   clap_event_header_t header;

   uint16_t       port_index;
   const uint8_t *buffer; // midiバッファ。上記の寿命コメントを参照。
   uint32_t       size;
} clap_event_midi_sysex_t;

// 一連のmidi2イベントを使用してsysexを送信することは可能ですが、
// 効率のために可能であればclap_event_midi_sysexを優先してください。
typedef struct clap_event_midi2 {
   clap_event_header_t header;

   uint16_t port_index;
   uint32_t data[4];
} clap_event_midi2_t;

// 入力イベントリスト。ホストはこれらをサンプル順にソートして配信します。
typedef struct clap_input_events {
   void *ctx; // リスト用の予約ポインター

   // リスト内のイベント数を返します
   uint32_t(CLAP_ABI *size)(const struct clap_input_events *list);

   // 返されたイベントを解放しないでください、それはリストに属しています
   const clap_event_header_t *(CLAP_ABI *get)(const struct clap_input_events *list, uint32_t index);
} clap_input_events_t;

// 出力イベントリスト。プラグインはイベントを挿入する際に、サンプルソート順でイベントを挿入する必要があります
typedef struct clap_output_events {
   void *ctx; // リスト用の予約ポインター

   // イベントのコピーをプッシュします
   // イベントがキューにプッシュできなかった場合（メモリ不足？）falseを返します
   bool(CLAP_ABI *try_push)(const struct clap_output_events *list,
                            const clap_event_header_t       *event);
} clap_output_events_t;

#ifdef __cplusplus
}
#endif