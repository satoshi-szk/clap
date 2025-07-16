#pragma once

#include "../string-sizes.h"
#include "../plugin.h"
#include "audio-ports.h"

/// @page Audio Ports Config
///
/// この拡張は、プラグインがポート構成プリセットを提供することを可能にします。
/// 例えば、モノ、ステレオ、サラウンド、アンビソニックなど。
///
/// プラグイン初期化後、ホストは構成のリストをスキャンし、
/// 最終的にプラグインコンテキストに適したものを選択できます。
/// ホストは、プラグインが非アクティブ化されている場合のみ構成を選択できます。
///
/// 構成はオーディオポートの非常にシンプルな記述です:
/// - メインの入力および出力ポートを記述します
/// - ユーザーに表示できる名前を持ちます
///
/// 構成の背後にある考え方は、ユーザーがメニューを介して1つを選択できるようにすることです。
///
/// 非常に複雑な構成の可能性を持つプラグインは、ユーザーがプラグインGUIから
/// ポートを構成し、@ref clap_host_audio_ports.rescan(CLAP_AUDIO_PORTS_RESCAN_ALL)を
/// 呼び出すべきです。
///
/// 正確なバスレイアウトを照会するために、プラグインはclap_plugin_audio_ports_config_info_t
/// 拡張を実装し、すべてのバスをaudio-port拡張と同じ方法で取得できます。

static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS_CONFIG[] = "clap.audio-ports-config";

static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS_CONFIG_INFO[] =
   "clap.audio-ports-config-info/1";

// 最新ドラフトは100%互換です。
// この互換IDは2026年に削除される可能性があります。
static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS_CONFIG_INFO_COMPAT[] =
   "clap.audio-ports-config-info/draft-0";

#ifdef __cplusplus
extern "C" {
#endif

// ポート構成の最小限の記述
typedef struct clap_audio_ports_config {
   clap_id id;
   char    name[CLAP_NAME_SIZE];

   uint32_t input_port_count;
   uint32_t output_port_count;

   // メイン入力情報
   bool        has_main_input;
   uint32_t    main_input_channel_count;
   const char *main_input_port_type;

   // メイン出力情報
   bool        has_main_output;
   uint32_t    main_output_channel_count;
   const char *main_output_port_type;
} clap_audio_ports_config_t;

// オーディオポート構成スキャンは、プラグインが非アクティブ化されている間に実行される必要があります。
typedef struct clap_plugin_audio_ports_config {
   // 利用可能な構成の数を取得します
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // 構成に関する情報を取得します
   // 成功時にtrueを返し、結果をconfigに格納します。
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t       *plugin,
                       uint32_t                   index,
                       clap_audio_ports_config_t *config);

   // idで指定された構成を選択します
   // 構成が適用できた場合trueを返します。
   // 適用されると、ホストはオーディオポートを再度スキャンする必要があります。
   // [main-thread & plugin-deactivated]
   bool(CLAP_ABI *select)(const clap_plugin_t *plugin, clap_id config_id);
} clap_plugin_audio_ports_config_t;

// 拡張構成情報
typedef struct clap_plugin_audio_ports_config_info {

   // 現在選択されている構成のidを取得します。現在のポートレイアウトが
   // 構成リストの一部でない場合はCLAP_INVALID_IDを返します。
   //
   // [main-thread]
   clap_id(CLAP_ABI *current_config)(const clap_plugin_t *plugin);

   // 指定されたconfig_idに対するオーディオポートの情報を取得します。
   // これはclap_plugin_audio_ports.get()に類似しています。
   // 成功時にtrueを返し、結果をinfoに格納します。
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t    *plugin,
                       clap_id                 config_id,
                       uint32_t                port_index,
                       bool                    is_input,
                       clap_audio_port_info_t *info);
} clap_plugin_audio_ports_config_info_t;

typedef struct clap_host_audio_ports_config {
   // 構成の完全なリストを再スキャンします。
   // [main-thread]
   void(CLAP_ABI *rescan)(const clap_host_t *host);
} clap_host_audio_ports_config_t;

#ifdef __cplusplus
}
#endif