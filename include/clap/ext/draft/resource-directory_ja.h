#pragma once

#include "../../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_RESOURCE_DIRECTORY[] = "clap.resource-directory/1";

#ifdef __cplusplus
extern "C" {
#endif

/// @page リソースディレクトリ
///
/// この拡張は、プラグインがホストによって提供されるディレクトリにリソースをファイルとして
/// 保存し、後でそれらを回復する方法を提供します。
///
/// プラグインは、リソースディレクトリに対する相対パスを状態内に保存**しなければなりません**。
///
/// リソース共有：
/// - 共有ディレクトリは、すべてのプラグインインスタンス間で共有されるため、主に読み取り専用
/// コンテンツに適しています
///   -> 読み取り専用コンテンツに適しています
/// - 排他ディレクトリは、プラグインインスタンス専用です
///   -> プラグインが複製される場合、その排他ディレクトリも複製されなければなりません
///   -> 読み書きコンテンツに適しています
///
/// 共有ディレクトリをクリーンに保つ：
/// - 共有ディレクトリでの衝突を避けるため、プラグインはファイルをサブフォルダーに
///   整理することが推奨されます。例えば、ベンダー名を使用して1つのサブディレクトリを作成します
/// - ディレクトリ外を指すシンボリックリンクやハードリンクは使用しないでください
///
/// リソースの寿命：
/// - 排他フォルダーコンテンツは、プラグインインスタンスによって管理されます
/// - 排他フォルダーコンテンツは、プラグインインスタンスがプロジェクトから削除されるときに削除されます
/// - 共有フォルダーコンテンツは、共有ディレクトリを使用するすべてのプラグインが
///   プロジェクトから削除されるまで、ホストによって管理されません
///
/// ホストへの注記
/// - 排他フォルダーの複製時にスペース使用量を削減するため、可能な場合はファイルシステムの
///   コピーオンライト機能を使用してください
/// - ホストは、以下を使用して共有フォルダー内のファイルを「ガベージコレクト」できます：
///     clap_plugin_resource_directory.get_files_count()
///     clap_plugin_resource_directory.get_file_path()
///   ただし、リソースを削除する前には**非常に**注意してください

typedef struct clap_plugin_resource_directory {
   // プラグインがそのリソースを保存できるディレクトリを設定します。
   // ディレクトリは、上書きされるかプラグインが破棄されるまで有効です。
   // pathがnullまたは空白の場合、ディレクトリ位置をクリアします。
   // pathは絶対パスでなければなりません。
   // [main-thread]
   void(CLAP_ABI *set_directory)(const clap_plugin_t *plugin, const char *path, bool is_shared);

   // プラグインにそのリソースをリソースディレクトリに配置するよう要求します。
   // allパラメータがtrueでない限り、プラグインのファクトリーコンテンツに
   // 属するファイルを収集する必要はありません。
   // [main-thread]
   void(CLAP_ABI *collect)(const clap_plugin_t *plugin, bool all);

   // 共有リソースフォルダー内でプラグインが使用するファイル数を返します。
   // [main-thread]
   uint32_t(CLAP_ABI *get_files_count)(const clap_plugin_t *plugin);

   // リソースディレクトリへの相対ファイルパスを取得します。
   // @param path パスを格納する書き込み可能なメモリ
   // @param path_size pathで利用可能なバイト数
   // パス内のバイト数を返し、エラー時は-1を返します
   // [main-thread]
   int32_t(CLAP_ABI *get_file_path)(const clap_plugin_t *plugin,
                                    uint32_t             index,
                                    char                *path,
                                    uint32_t             path_size);
} clap_plugin_resource_directory_t;

typedef struct clap_host_resource_directory {
   // 指定された共有でリソースディレクトリをセットアップするようホストに要求します。
   // ホストがリクエストを実行する場合はtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *request_directory)(const clap_host_t *host, bool is_shared);

   // 指定された共有のリソースディレクトリがもはや必要ないことをホストに伝えます。
   // is_shared = falseの場合、ホストはディレクトリコンテンツを削除する可能性があります。
   // [main-thread]
   void(CLAP_ABI *release_directory)(const clap_host_t *host, bool is_shared);
} clap_host_resource_directory_t;

#ifdef __cplusplus
}
#endif