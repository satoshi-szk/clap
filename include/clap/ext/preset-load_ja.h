#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_PRESET_LOAD[] = "clap.preset-load/2";

// 最新ドラフトは100%互換です。
// この互換IDは2026年に削除される可能性があります。
static CLAP_CONSTEXPR const char CLAP_EXT_PRESET_LOAD_COMPAT[] = "clap.preset-load.draft/2";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_preset_load {
   // プラグインネイティブプリセットファイル形式でプリセットを場所からロードします。
   // プリセット発見プロバイダーは、この関数に渡される場所とload_keyを定義します。
   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *from_location)(const clap_plugin_t *plugin,
                                 uint32_t             location_kind,
                                 const char          *location,
                                 const char          *load_key);
} clap_plugin_preset_load_t;

typedef struct clap_host_preset_load {
   // clap_plugin_preset_load.load()が失敗した場合に呼び出されます。
   // os_error: 該当する場合、オペレーティングシステムエラー。
   // 該当しない場合は、非エラー値に設定してください。例: unixとWindowsでは0。
   //
   // [main-thread]
   void(CLAP_ABI *on_error)(const clap_host_t *host,
                            uint32_t           location_kind,
                            const char        *location,
                            const char        *load_key,
                            int32_t            os_error,
                            const char        *msg);

   // 以下のプリセットがロードされたことをホストに通知します。
   // これは、ホストプリセットブラウザーとプラグインプリセットブラウザーの
   // 同期を保つのに貢献します。
   // プリセットがコンテナファイルからロードされた場合は、load_keyを設定する必要があります。
   // そうでなければnullである必要があります。
   //
   // [main-thread]
   void(CLAP_ABI *loaded)(const clap_host_t *host,
                          uint32_t           location_kind,
                          const char        *location,
                          const char        *load_key);
} clap_host_preset_load_t;

#ifdef __cplusplus
}
#endif