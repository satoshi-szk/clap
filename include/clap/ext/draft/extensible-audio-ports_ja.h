#pragma once

#include "../audio-ports.h"

#ifdef __cplusplus
extern "C" {
#endif

// この拡張により、ホストはプラグインにオーディオポートを追加および削除できます。
static CLAP_CONSTEXPR const char CLAP_EXT_EXTENSIBLE_AUDIO_PORTS[] =
   "clap.extensible-audio-ports/1";

typedef struct clap_plugin_extensible_audio_ports {
   // プラグインに以下の設定で新しいポート（リストの最後）を追加するよう要求します。
   // port_type: 解釈についてはclap_audio_port_info.port_typeを参照してください。
   // port_details: 解釈についてはclap_audio_port_configuration_request.port_detailsを参照してください。
   // 成功時にtrueを返します。
   // [main-thread && !is_active]
   bool(CLAP_ABI *add_port)(const clap_plugin_t *plugin,
                            bool                 is_input,
                            uint32_t             channel_count,
                            const char          *port_type,
                            const void          *port_details);

   // プラグインにポートを削除するよう要求します。
   // 成功時にtrueを返します。
   // [main-thread && !is_active]
   bool(CLAP_ABI *remove_port)(const clap_plugin_t *plugin, bool is_input, uint32_t index);
} clap_plugin_extensible_audio_ports_t;

#ifdef __cplusplus
}
#endif