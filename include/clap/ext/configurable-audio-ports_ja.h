#pragma once

#include "audio-ports.h"

#ifdef __cplusplus
extern "C" {
#endif

// この拡張は、ホストがプラグインの入力および出力オーディオポートを構成することを可能にします。
// これはオーディオポート構成への「プッシュ」アプローチです。

static CLAP_CONSTEXPR const char CLAP_EXT_CONFIGURABLE_AUDIO_PORTS[] =
   "clap.configurable-audio-ports/1";

// 最新ドラフトは100%互換です。
// この互換IDは2026年に削除される可能性があります。
static CLAP_CONSTEXPR const char CLAP_EXT_CONFIGURABLE_AUDIO_PORTS_COMPAT[] =
   "clap.configurable-audio-ports.draft1";

typedef struct clap_audio_port_configuration_request {
   // is_inputとport_indexによってポートを識別します
   bool     is_input;
   uint32_t port_index;

   // 要求されるチャンネル数。
   uint32_t channel_count;

   // ポートタイプ、audio-ports.h、clap_audio_port_info.port_typeの解釈を参照してください。
   const char *port_type;

   // port_typeに従ってport_detailsをキャストします:
   // - CLAP_PORT_MONO: (破棄)
   // - CLAP_PORT_STEREO: (破棄)
   // - CLAP_PORT_SURROUND: const uint8_t *channel_map
   // - CLAP_PORT_AMBISONIC: const clap_ambisonic_config_t *info
   const void *port_details;
} clap_audio_port_configuration_request_t;

typedef struct clap_plugin_configurable_audio_ports {
   // apply_configuration()を使用して指定された構成を適用できる場合trueを返します。
   // [main-thread && !active]
   bool(CLAP_ABI *can_apply_configuration)(
      const clap_plugin_t                                *plugin,
      const struct clap_audio_port_configuration_request *requests,
      uint32_t                                            request_count);

   // 一連の構成要求を送信し、それらがアトミックに一緒に適用されるか、
   // 一緒に破棄されます。
   //
   // 構成が正常に適用されると、プラグインがclap_host_audio_ports->changed()を
   // 呼び出す必要はなく、ホストがオーディオポートをスキャンする必要もありません。
   //
   // 適用された場合trueを返します。
   // [main-thread && !active]
   bool(CLAP_ABI *apply_configuration)(const clap_plugin_t                                *plugin,
                                       const struct clap_audio_port_configuration_request *requests,
                                       uint32_t request_count);
} clap_plugin_configurable_audio_ports_t;

#ifdef __cplusplus
}
#endif