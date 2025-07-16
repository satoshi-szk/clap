#pragma once

#include "../plugin.h"
#include "../string-sizes.h"

/// @page Audio Ports
///
/// この拡張は、プラグインが現在のオーディオポートを記述する方法を提供します。
///
/// プラグインがこの拡張を実装しない場合、オーディオポートを持ちません。
///
/// 32ビットサポートは、ホストとプラグインの両方に必要です。64ビットオーディオはオプションです。
///
/// プラグインは、非アクティブ化されている間のみポート構成を変更することが許可されています。

static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS[] = "clap.audio-ports";
static CLAP_CONSTEXPR const char CLAP_PORT_MONO[] = "mono";
static CLAP_CONSTEXPR const char CLAP_PORT_STEREO[] = "stereo";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // このポートはメインオーディオ入力または出力です。
   // メイン入力とメイン出力は1つずつしか存在できません。
   // メインポートはインデックス0である必要があります。
   CLAP_AUDIO_PORT_IS_MAIN = 1 << 0,

   // このポートは64ビットオーディオで使用できます
   CLAP_AUDIO_PORT_SUPPORTS_64BITS = 1 << 1,

   // このポートでは64ビットオーディオが推奨されます
   CLAP_AUDIO_PORT_PREFERS_64BITS = 1 << 2,

   // このポートは、このフラグを持つ他のすべてのポートと同じサンプルサイズで使用される必要があります。
   // 言い換えると、すべてのポートがこのフラグを持つ場合、プラグインは
   // 64ビットオーディオまたは32ビットオーディオで完全に使用される可能性がありますが、混在はできません。
   CLAP_AUDIO_PORT_REQUIRES_COMMON_SAMPLE_SIZE = 1 << 3,
};

typedef struct clap_audio_port_info {
   // idはポートを識別し、安定している必要があります。
   // idは入力ポートと出力ポート間で重複する可能性があります。
   clap_id id;
   char    name[CLAP_NAME_SIZE]; // 表示可能な名前

   uint32_t flags;
   uint32_t channel_count;

   // nullまたは空の場合、未指定（任意のオーディオ）です。
   // このフィールドは以下と比較できます:
   // - CLAP_PORT_MONO
   // - CLAP_PORT_STEREO
   // - CLAP_PORT_SURROUND (surround拡張で定義)
   // - CLAP_PORT_AMBISONIC (ambisonic拡張で定義)
   //
   // 拡張は独自のポートタイプとチャンネルを検査する方法を提供できます。
   const char *port_type;

   // インプレース処理: ホストが入力と出力に同じバッファを使用することを許可
   // サポートされている場合はペアポートIDを設定します。
   // サポートされていない場合はCLAP_INVALID_IDに設定します
   clap_id in_place_pair;
} clap_audio_port_info_t;

// オーディオポートスキャンは、プラグインが非アクティブ化されている間に実行される必要があります。
typedef struct clap_plugin_audio_ports {
   // 入力または出力のいずれかのポート数
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin, bool is_input);

   // オーディオポートに関する情報を取得します。
   // 成功時にtrueを返し、結果をinfoに格納します。
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t    *plugin,
                       uint32_t                index,
                       bool                    is_input,
                       clap_audio_port_info_t *info);
} clap_plugin_audio_ports_t;

enum {
   // ポート名が変更されました。ホストはすぐにスキャンできます。
   CLAP_AUDIO_PORTS_RESCAN_NAMES = 1 << 0,

   // [!active] フラグが変更されました
   CLAP_AUDIO_PORTS_RESCAN_FLAGS = 1 << 1,

   // [!active] channel_countが変更されました
   CLAP_AUDIO_PORTS_RESCAN_CHANNEL_COUNT = 1 << 2,

   // [!active] ポートタイプが変更されました
   CLAP_AUDIO_PORTS_RESCAN_PORT_TYPE = 1 << 3,

   // [!active] インプレースペアが変更されました。これが必要です。
   CLAP_AUDIO_PORTS_RESCAN_IN_PLACE_PAIR = 1 << 4,

   // [!active] ポートのリストが変更されました: エントリが削除/追加されました。
   CLAP_AUDIO_PORTS_RESCAN_LIST = 1 << 5,
};

typedef struct clap_host_audio_ports {
   // ホストがプラグインにオーディオポート定義の特定の側面の変更を許可するかどうかをチェックします。
   // [main-thread]
   bool(CLAP_ABI *is_rescan_flag_supported)(const clap_host_t *host, uint32_t flag);

   // フラグに従ってオーディオポートの完全なリストを再スキャンします。
   // サポートされていないフラグでホストに再スキャンを要求することは違法です。
   // 特定のフラグでは、プラグインが非アクティブ化されている必要があります。
   // [main-thread]
   void(CLAP_ABI *rescan)(const clap_host_t *host, uint32_t flags);
} clap_host_audio_ports_t;

#ifdef __cplusplus
}
#endif