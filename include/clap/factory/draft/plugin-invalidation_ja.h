#pragma once

#include "../../private/std.h"
#include "../../private/macros.h"

// clap_plugin_entry.get_factory()からconst clap_plugin_invalidation_factory_t*を取得するために使用
static const CLAP_CONSTEXPR char CLAP_PLUGIN_INVALIDATION_FACTORY_ID[] =
   "clap.plugin-invalidation-factory/1";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_invalidation_source {
   // スキャンするファイルを含むディレクトリ、絶対パスでなければなりません
   const char *directory;

   // グロブパターン、*.dllの形式
   const char *filename_glob;

   // ディレクトリを再帰的にスキャンする必要がありますか？
   bool recursive_scan;
} clap_plugin_invalidation_source_t;

// プラグインをいつ再スキャンする必要があるかを判断するために使用されます。
// 単一のエントリポイント（my-plugin.clap）があり、自分自身を「サブプラグイン」の
// セットとしてスキャンする状況を想像してください。my-plugin.clapファイルが変更
// されなくても、新しいプラグインが利用可能になる場合があります。
// このインターフェースはこの問題を解決し、ホストが追加ファイルを監視する方法を提供します。
typedef struct clap_plugin_invalidation_factory {
   // 無効化ソースの数を取得します。
   uint32_t(CLAP_ABI *count)(const struct clap_plugin_invalidation_factory *factory);

   // インデックスによって無効化ソースを取得します。
   // [thread-safe]
   const clap_plugin_invalidation_source_t *(CLAP_ABI *get)(
      const struct clap_plugin_invalidation_factory *factory, uint32_t index);

   // ホストが無効化イベントを検出した場合、refresh()を呼び出して
   // plugin_entryが利用可能なプラグインのセットを更新できるようにします。
   // 関数がfalseを返した場合、プラグインを再読み込みする必要があります。
   bool(CLAP_ABI *refresh)(const struct clap_plugin_invalidation_factory *factory);
} clap_plugin_invalidation_factory_t;

#ifdef __cplusplus
}
#endif