#pragma once

#include "../plugin.h"
#include "../string-sizes.h"

/// @page パラメータ
/// @brief パラメータ管理
///
/// 主な概念：
///
/// ホストはプラグインを単一のエンティティとして扱い、そのパラメータ上でコントローラーとして動作します。
/// プラグインは、オーディオプロセッサとGUIの同期を保つ責任があります。
///
/// ホストは、[メインスレッド]で@ref clap_plugin_params.get_value()を使用して、
/// いつでもパラメータの値を読み取ることができます。
///
/// パラメータ値の変更を通信するには2つの方法があり、これらは並行して使用されません：
/// - clap_plugin.process()中にオートメーションポイントを送信する
/// - オーディオ処理なしでパラメータ変更を行う場合、clap_plugin_params.flush()中に
///   オートメーションポイントを送信する
///
/// プラグインがパラメータ値を変更する場合、ホストに通知する必要があります。
/// process()またはflush()中に@ref CLAP_EVENT_PARAM_VALUEイベントを送信します。
/// ユーザーが値を調整している場合は、CLAP_EVENT_PARAM_GESTURE_BEGINと
/// CLAP_EVENT_PARAM_GESTURE_ENDイベントを送信して、ジェスチャーの開始と終了を
/// マークすることを忘れないでください。
///
/// @note MIDI CCは、パラメータ調整がいつ終了するかわからない場合があるため、扱いが難しいです。
/// また、ホストが受信MIDI CCとパラメータ変更のオートメーションを同時に記録する場合、
/// 再生時に競合が発生します：MIDI CC vs オートメーション。
/// パラメータオートメーションは、param_idが安定しているため、常に同じパラメータを対象とします。
/// MIDI CCは将来異なるマッピングを持つ可能性があり、異なる再生結果になる可能性があります。
///
/// MIDI CCがパラメータの値を変更する場合、clap_event_param.header.flagsに
/// CLAP_EVENT_DONT_RECORDフラグを設定してください。これにより、ホストはMIDI CCの
/// オートメーションを記録できますが、パラメータ変更は記録されず、再生時に競合が発生しません。
///
/// シナリオ：
///
/// I. プリセットの読み込み
/// - プリセットを一時的な状態に読み込む
/// - 何か変更があった場合は@ref clap_host_params.rescan()を呼び出す
/// - レイテンシが変更された場合はclap_host_latency.changed()を呼び出す
/// - ホストがキャッシュしている可能性のある他の情報を無効化する
/// - プラグインがアクティブで、プリセットが破壊的な変更を導入する場合
///   （レイテンシ、オーディオポート、新しいパラメータなど）、ホストが
///   プラグインを非アクティブ化するまで待ってから、これらの変更を適用してください。
///   破壊的な変更がない場合、プラグインはすぐに適用できます。
///   プラグインは、オーディオプロセッサとGUIの両方を更新する責任があります。
///
/// II. DAWインターフェース上でノブを回す
/// - ホストはprocess()またはflush()を介してプラグインにオートメーションイベントを送信します
///
/// III. プラグインインターフェース上でノブを回す
/// - プラグインはパラメータ値をオーディオプロセッサに送信する責任があります
/// - clap_host_params->request_flush()またはclap_host->request_process()を呼び出す
/// - ホストがclap_plugin->process()またはclap_plugin_params->flush()を呼び出したとき、
///   オートメーションイベントを送信し、ジェスチャーの開始と終了を定義するために
///   CLAP_EVENT_PARAM_GESTURE_BEGINとCLAP_EVENT_PARAM_GESTURE_ENDで
///   パラメータ変更をラップすることを忘れないでください。
///
/// IV. オートメーションによるノブの回転
/// - ホストはclap_plugin->process()またはclap_plugin_params->flush()中に
///   オートメーションポイントを送信します。
/// - プラグインはGUIを更新する責任があります
///
/// V. プラグインの内部MIDIマッピングによるノブの回転
/// - プラグインはCLAP_EVENT_PARAM_VALUE出力イベントを送信し、should_recordをfalseに設定します
/// - プラグインはGUIを更新する責任があります
///
/// VI. パラメータの追加または削除
/// - プラグインがアクティブな場合はclap_host->restart()を呼び出す
/// - プラグインがアクティブでなくなったら：
///   - 新しい状態を適用する
///   - パラメータが削除されたか、以前使用された可能性のあるIDで作成された場合、
///     clap_host_params.clear(host, param_id, CLAP_PARAM_CLEAR_ALL)を呼び出す
///   - clap_host_params->rescan(CLAP_PARAM_RESCAN_ALL)を呼び出す
///
/// CLAPはプラグインがパラメータの範囲を変更することを許可していますが、
/// プラグイン開発者は、特に音を変更しないという約束をした場合、
/// それがリスクなしではないことを認識する必要があります。
/// すべてのホストで音が変更されないことを100％確実にしたい場合は、
/// 単に範囲を決して変更しないでください。
///
/// オートメーションには2つのアプローチがあります。プレーン値を自動化するか、
/// ノブの位置を自動化するかです。最初のオプションは範囲の増加に対して堅牢ですが、
/// 2番目はそうではありません。
///
/// ホストが2番目のアプローチ（ノブ位置の自動化）を採用する場合、
/// プラグインは音の変更に関してリラックスした環境でホストされていることを意味します
/// （合理的である限り、変更は受け入れられ、懸念事項ではありません）。
/// ただし、ステップパラメータはドキュメントにプレーン値として保存する必要があります。
///
/// ホストが最初のアプローチを採用しても、音が必然的に変更される状況があります。
/// たとえば、プラグインが範囲を増やし、最大値でオートメーションが再生されており、
/// その上にLFOが適用されている場合。
/// 次の曲線を参照してください：
///                                   .
///                                  . .
///          .....                  .   .
/// 変更前: .     .     変更後: .     .
///
/// パラメータ値の永続化：
///
/// プラグインは、state拡張を実装することで、セッション間でパラメータ値を
/// 永続化する責任があります。そうしないと、プロジェクトを再読み込みしたときに
/// パラメータ値が復元されません。ホストは、state拡張を実装していないプラグインの
/// パラメータ値を保存および復元しようとすべきではありません。
///
/// ホストへのアドバイス：
///
/// - ドキュメント（オートメーション）にプレーン値を保存する
/// - 変調量をパーセンテージではなくプレーン値デルタで保存する
/// - CCマッピングを適用するとき、調整できるように最小/最大プレーン値を記憶する
/// - state拡張を実装していないプラグインのパラメータ保存フォールバックを実装しない
///
/// プラグインへのアドバイス：
///
/// - DSPを設計する際、パラメータ範囲について慎重に考える
/// - パラメータ範囲の縮小を避ける、音が変更される可能性が非常に高い
/// - パラメータ範囲の変更をトレードオフとして考える：改善するもの vs 壊すもの
/// - state拡張を使用してパラメータ値の保存と読み込みを実装することを確認する
/// - 他のプラグインフォーマット用のアダプターを使用する予定がある場合、
///   アダプターの要件に特別な注意を払う必要がある
cstatic CLAP_CONSTEXPR const char CLAP_EXT_PARAMS[] = "clap.params";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // このパラメータはステップ化されているか？（整数値のみ）
   // その場合、double値はキャスト（truncに相当）を使用して整数に変換されます。
   CLAP_PARAM_IS_STEPPED = 1 << 0,

   // 位相のような周期的パラメータに便利
   CLAP_PARAM_IS_PERIODIC = 1 << 1,

   // パラメータは現在使用されていないため、ユーザーに表示すべきではありません。
   // このパラメータのオートメーションを処理する必要はありません。
   CLAP_PARAM_IS_HIDDEN = 1 << 2,

   // パラメータはホストによって変更できません。
   CLAP_PARAM_IS_READONLY = 1 << 3,

   // このパラメータは、プラグインとホストのバイパスボタンをマージするために使用されます。
   // パラメータがステップ化されていることを意味します。
   // min: 0 -> バイパスオフ
   // max: 1 -> バイパスオン
   CLAP_PARAM_IS_BYPASS = 1 << 4,

   // 設定時：
   // - オートメーションを記録できる
   // - オートメーションを再生できる
   //
   // ホストは、このフラグに関係なく、このパラメータのライブユーザー変更を送信できます。
   //
   // このパラメータがプラグインの内部処理構造に影響する場合（最大遅延、FFTサイズなど）、
   // プラグインが作業バッファを再割り当てする必要がある場合は、
   // host->request_restart()を呼び出し、プラグインが再アクティブ化されたら変更を実行する必要があります。
   CLAP_PARAM_IS_AUTOMATABLE = 1 << 5,

   // このパラメータはノートごとのオートメーションをサポートしますか？
   CLAP_PARAM_IS_AUTOMATABLE_PER_NOTE_ID = 1 << 6,

   // このパラメータはキーごとのオートメーションをサポートしますか？
   CLAP_PARAM_IS_AUTOMATABLE_PER_KEY = 1 << 7,

   // このパラメータはチャンネルごとのオートメーションをサポートしますか？
   CLAP_PARAM_IS_AUTOMATABLE_PER_CHANNEL = 1 << 8,

   // このパラメータはポートごとのオートメーションをサポートしますか？
   CLAP_PARAM_IS_AUTOMATABLE_PER_PORT = 1 << 9,

   // このパラメータは変調信号をサポートしますか？
   CLAP_PARAM_IS_MODULATABLE = 1 << 10,

   // このパラメータはノートごとの変調をサポートしますか？
   CLAP_PARAM_IS_MODULATABLE_PER_NOTE_ID = 1 << 11,

   // このパラメータはキーごとの変調をサポートしますか？
   CLAP_PARAM_IS_MODULATABLE_PER_KEY = 1 << 12,

   // このパラメータはチャンネルごとの変調をサポートしますか？
   CLAP_PARAM_IS_MODULATABLE_PER_CHANNEL = 1 << 13,

   // このパラメータはポートごとの変調をサポートしますか？
   CLAP_PARAM_IS_MODULATABLE_PER_PORT = 1 << 14,

   // このパラメータへの変更はプラグインの出力に影響し、
   // プラグインがアクティブな場合はprocess()を介して行う必要があります。
   //
   // 簡単な例はDCオフセットです。変更すると出力信号が変更され、処理する必要があります。
   CLAP_PARAM_REQUIRES_PROCESS = 1 << 15,

   // このパラメータは列挙値を表します。
   // このフラグを設定する場合、CLAP_PARAM_IS_STEPPEDも設定する必要があります。
   // minからmaxまでのすべての値は、空白のvalue_to_text()を持ってはいけません。
   CLAP_PARAM_IS_ENUM = 1 << 16,
};
typedef uint32_t clap_param_info_flags;

/* パラメータの説明 */
typedef struct clap_param_info {
   // 安定したパラメータ識別子、決して変更してはいけません。
   clap_id id;

   clap_param_info_flags flags;

   // この値はオプションで、プラグインによって設定されます。
   // その目的は、ポインタをキャッシュすることで、プラグインパラメータオブジェクトへの高速アクセスを提供することです。
   // 例：
   //
   // clap_plugin_params.get_info()内：
   //    Parameter *p = findParameter(param_id);
   //    param_info->cookie = p;
   //
   // 後で、clap_plugin.process()内：
   //
   //    Parameter *p = (Parameter *)event->cookie;
   //    if (!p) [[unlikely]]
   //       p = findParameter(event->param_id);
   //
   // ここで、findParameter()はパラメータIDを内部オブジェクトにマップするために
   // プラグインが実装する関数です。
   //
   // 重要：
   //  - cookieは、clap_host_params->rescan(CLAP_PARAM_RESCAN_ALL)の呼び出しまたは
   //    プラグインが破棄されたときに無効化されます。
   //  - ホストは、パラメータを対象とするイベントで、発行されたcookieまたはnullptrを提供します。
   //  - プラグインは、nullptrであるcookieのケースを優雅に処理する必要があります。
   //  - 多くのプラグインは、ホストがイベントごとのパラメータごとのハッシュマップ検索よりも
   //    高速にcookieを提供できる場合、パラメータイベントをより迅速に処理します。
   void *cookie;

   // 表示名。例："Volume"。これは一意である必要はありません。
   // モジュールテキストを含めないでください。ホストは、名前だけを表示すると
   // あいまいすぎる場合に、モジュール+名前を連結/フォーマットする必要があります。
   char name[CLAP_NAME_SIZE];

   // パラメータを含むモジュールパス。例："Oscillators/Wavetable 1"。
   // '/'は、ツリー状の構造を表示するためのセパレータとして使用されます。
   char module[CLAP_PATH_SIZE];

   double min_value;     // 最小プレーン値。有限でなければなりません（`std::isfinite`がtrue）
   double max_value;     // 最大プレーン値。有限でなければなりません
   double default_value; // デフォルトプレーン値。[min, max]の範囲内でなければなりません。
} clap_param_info_t;

typedef struct clap_plugin_params {
   // パラメータの数を返します。
   // [メインスレッド]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // パラメータの情報をparam_infoにコピーします。
   // 成功時にtrueを返します。
   // [メインスレッド]
   bool(CLAP_ABI *get_info)(const clap_plugin_t *plugin,
                            uint32_t             param_index,
                            clap_param_info_t   *param_info);

   // パラメータの現在の値をout_valueに書き込みます。
   // 成功時にtrueを返します。
   // [メインスレッド]
   bool(CLAP_ABI *get_value)(const clap_plugin_t *plugin, clap_id param_id, double *out_value);

   // out_bufferを、指定された'value'引数でパラメータを表すnull終端UTF-8文字列で埋めます。
   // 例："2.3 kHz"。ホストは、ユーザーに表示する前に、常にこれを使用して
   // パラメータ値をフォーマットする必要があります。
   // 成功時にtrueを返します。
   // [メインスレッド]
   bool(CLAP_ABI *value_to_text)(const clap_plugin_t *plugin,
                                 clap_id              param_id,
                                 double               value,
                                 char                *out_buffer,
                                 uint32_t             out_buffer_capacity);

   // null終端UTF-8 param_value_textをdoubleに変換し、out_valueに書き込みます。
   // ホストはこれを使用して、ユーザー入力をパラメータ値に変換できます。
   // 成功時にtrueを返します。
   // [メインスレッド]
   bool(CLAP_ABI *text_to_value)(const clap_plugin_t *plugin,
                                 clap_id              param_id,
                                 const char          *param_value_text,
                                 double              *out_value);

   // パラメータ変更のセットをフラッシュします。
   // このメソッドは、clap_plugin->process()と同時に呼び出してはいけません。
   //
   // 注：プラグインが処理中の場合、process()呼び出しはすでにパラメータ更新を
   // 達成しているため（双方向）、flush()の呼び出しは必要ありません。
   // また、プラグインはprocess()でサンプルオフセットを使用する可能性がありますが、
   // この情報はflush()内で失われることに注意してください。
   //
   // [アクティブ ? オーディオスレッド : メインスレッド]
   void(CLAP_ABI *flush)(const clap_plugin_t        *plugin,
                         const clap_input_events_t  *in,
                         const clap_output_events_t *out);
} clap_plugin_params_t;

enum {
   // パラメータ値が変更されました。例：プリセットの読み込み後。
   // ホストはすべてのパラメータ値をスキャンします。
   // ホストはこれらの変更をオートメーションポイントとして記録しません。
   // 新しい値はすぐに有効になります。
   CLAP_PARAM_RESCAN_VALUES = 1 << 0,

   // 値からテキストへの変換が変更され、テキストを再レンダリングする必要があります。
   CLAP_PARAM_RESCAN_TEXT = 1 << 1,

   // パラメータ情報が変更されました。以下の場合にこのフラグを使用します：
   // - 名前の変更
   // - モジュールの変更
   // - is_periodic（フラグ）
   // - is_hidden（フラグ）
   // 新しい情報はすぐに有効になります。
   CLAP_PARAM_RESCAN_INFO = 1 << 2,

   // ホストがパラメータについて知っているすべてを無効化します。
   // プラグインが非アクティブ化されている間のみ使用できます。
   // プラグインがアクティブ化されている場合は、clap_host->restart()を使用し、
   // ホストがclap_plugin->deactivate()を呼び出すまで変更を遅延させます。
   //
   // 以下の場合にこのフラグを使用する必要があります：
   // - いくつかのパラメータが追加または削除された
   // - いくつかのパラメータに重要な変更があった：
   //   - is_per_note（フラグ）
   //   - is_per_key（フラグ）
   //   - is_per_channel（フラグ）
   //   - is_per_port（フラグ）
   //   - is_readonly（フラグ）
   //   - is_bypass（フラグ）
   //   - is_stepped（フラグ）
   //   - is_modulatable（フラグ）
   //   - min_value
   //   - max_value
   //   - cookie
   CLAP_PARAM_RESCAN_ALL = 1 << 3,
};
typedef uint32_t clap_param_rescan_flags;

enum {
   // パラメータへのすべての可能な参照をクリアする
   CLAP_PARAM_CLEAR_ALL = 1 << 0,

   // パラメータへのすべてのオートメーションをクリアする
   CLAP_PARAM_CLEAR_AUTOMATIONS = 1 << 1,

   // パラメータへのすべての変調をクリアする
   CLAP_PARAM_CLEAR_MODULATIONS = 1 << 2,
};
typedef uint32_t clap_param_clear_flags;

typedef struct clap_host_params {
   // フラグに従ってパラメータの完全なリストを再スキャンします。
   // [メインスレッド]
   void(CLAP_ABI *rescan)(const clap_host_t *host, clap_param_rescan_flags flags);

   // パラメータへの参照をクリアします。
   // [メインスレッド]
   void(CLAP_ABI *clear)(const clap_host_t *host, clap_id param_id, clap_param_clear_flags flags);

   // パラメータフラッシュをリクエストします。
   //
   // ホストは次のいずれかへの呼び出しをスケジュールします：
   // - clap_plugin.process()
   // - clap_plugin_params.flush()
   //
   // この関数は常に安全に使用でき、[オーディオスレッド]から呼び出すべきではありません。
   // プラグインはすでにprocess()またはflush()内にいるためです。
   //
   // [スレッドセーフ、!オーディオスレッド]
   void(CLAP_ABI *request_flush)(const clap_host_t *host);
} clap_host_params_t;

#ifdef __cplusplus
}
#endif