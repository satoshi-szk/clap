#pragma once

#include "../plugin.h"

// clap_plugin_entry.get_factory()からconst clap_plugin_factory_t*を取得するために使用
static const CLAP_CONSTEXPR char CLAP_PLUGIN_FACTORY_ID[] = "clap.plugin-factory";

#ifdef __cplusplus
extern "C" {
#endif

// 全てのメソッドはスレッドセーフでなければなりません。
// プラグインを可能な限り迅速にスキャンできることが非常に重要です。
//
// ホストは、ファクトリーのコンテンツを変更する可能性のあるファイルシステムの変更を
// 検出するためにclap_plugin_invalidation_factoryを使用する場合があります。
typedef struct clap_plugin_factory {
   // 利用可能なプラグイン数を取得します。
   // [thread-safe]
   uint32_t(CLAP_ABI *get_plugin_count)(const struct clap_plugin_factory *factory);

   // インデックスによってプラグインディスクリプタを取得します。
   // エラーの場合nullを返します。
   // ディスクリプタを解放してはいけません。
   // [thread-safe]
   const clap_plugin_descriptor_t *(CLAP_ABI *get_plugin_descriptor)(
      const struct clap_plugin_factory *factory, uint32_t index);

   // plugin_idによってclap_pluginを作成します。
   // 返されたポインタは、plugin->destroy(plugin);を呼び出すことで解放する必要があります。
   // プラグインは、createメソッド内でホストコールバックを使用することは許可されていません。
   // エラーの場合nullを返します。
   // [thread-safe]
   const clap_plugin_t *(CLAP_ABI *create_plugin)(const struct clap_plugin_factory *factory,
                                                  const clap_host_t                *host,
                                                  const char                       *plugin_id);
} clap_plugin_factory_t;

#ifdef __cplusplus
}
#endif