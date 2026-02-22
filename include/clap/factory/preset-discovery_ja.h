/*
  プリセット発見API。

  プリセット発見は、プラグインホストがプリセットの場所、拡張子、
  適用されるプラグイン、プリセットに関連付けられたその他のメタデータを
  特定することを可能にし、プラグインホストのブラウザー内で迅速にインデックス化および検索できるようにします。

  これにより、ユーザーには多くの利点があります：
  - 一つの中央の場所から、一貫した方法でプリセットを閲覧できます
  - ユーザーは特定のプラグインを最初に選択することなく、プリセットを閲覧できます

  APIは以下のようにして、プリセットとプリセットメタデータをインデックス化します：
  1. clap_plugin_entry.get_factory(CLAP_PRESET_DISCOVERY_FACTORY_ID)
  2. clap_preset_discovery_factory_t.create(...)
  3. clap_preset_discovery_provider.init() (初回のみ必要、宣言は
  キャッシュ可能)
       `-> clap_preset_discovery_indexer.declare_filetype()
       `-> clap_preset_discovery_indexer.declare_location()
       `-> clap_preset_discovery_indexer.declare_soundpack() (optional)
       `-> clap_preset_discovery_indexer.set_invalidation_watch_file() (optional)
  4. 指定された場所をクロールし、ファイルシステムの変更を監視
       `-> 各プリセットファイルに対してclap_preset_discovery_indexer.get_metadata()

  その後、プリセットをロードするには、ext/draft/preset-load.hを使用します。
  TODO: 他のプラグインabiプリセットロード拡張用の専用リポジトリを作成。

  このAPIの設計は、意図的に固定されたタグやカテゴリのセットを定義していません。プリセットで
  見つかった生の機能リストをインテリジェントにマッピングし、ホストのタグ付けおよび
  分類システムに適したものを生成するために、このリストを処理することは
  プラグインホストの仕事です。この理由は、プラグイン開発者が既存のプリセットファイル形式に
  プリセット発見サポートを追加する作業を削減し、すべての異なるホストや
  それらがメタデータを受信したい方法について心配する必要がないようにするためです。

  非常に重要：
  - 全体のインデックス化プロセスは**高速**でなければなりません
     - clap_preset_provider->get_metadata()は高速で、不必要な操作を避ける必要があります
  - 全体のインデックス化プロセスはインタラクティブであってはいけません
     - ダイアログ、ウィンドウなどを表示しない
     - ユーザー入力を求めない
*/

#pragma once

#include "../private/std.h"
#include "../private/macros.h"
#include "../timestamp.h"
#include "../version.h"
#include "../universal-plugin-id.h"

// clap_plugin_entry.get_factory()からconst clap_preset_discovery_factory_t*を取得するために使用
static const CLAP_CONSTEXPR char CLAP_PRESET_DISCOVERY_FACTORY_ID[] =
   "clap.preset-discovery-factory/2";

// 最新のドラフトは100%互換性があります。
// この互換性IDは2026年に削除される可能性があります。
static const CLAP_CONSTEXPR char CLAP_PRESET_DISCOVERY_FACTORY_ID_COMPAT[] =
   "clap.preset-discovery-factory/draft-2";

#ifdef __cplusplus
extern "C" {
#endif

enum clap_preset_discovery_location_kind {
   // プリセットはOSファイルシステム上のファイルに配置されています。
   // 場所は、OSファイルシステム関数（open、stat、...）で動作するパスです。
   // そのため、Windowsでは'/'と'\'の両方がセパレーターとして動作する必要があります。
   CLAP_PRESET_DISCOVERY_LOCATION_FILE = 0,

   // プリセットはプラグインDSO自体にバンドルされています。
   // プリセットがプラグイン自体の中にあり、プラグインが
   // プリセットコンテナとして機能するため、場所はnullでなければなりません。
   CLAP_PRESET_DISCOVERY_LOCATION_PLUGIN = 1,
};

enum clap_preset_discovery_flags {
   // これはファクトリーまたはサウンドパックプリセット用です。
   CLAP_PRESET_DISCOVERY_IS_FACTORY_CONTENT = 1 << 0,

   // これはユーザープリセット用です。
   CLAP_PRESET_DISCOVERY_IS_USER_CONTENT = 1 << 1,

   // この場所はデモプリセット用で、ユーザーが
   // 購入する必要がある追加機能を必要とするか、コンテンツ自体を
   // 購入する必要があり、デモモードでのみ利用可能であるため、
   // プラグインでいくつかの制限を引き起こす可能性があるプリセットです。
   CLAP_PRESET_DISCOVERY_IS_DEMO_CONTENT = 1 << 2,

   // このプリセットはユーザーのお気に入りです
   CLAP_PRESET_DISCOVERY_IS_FAVORITE = 1 << 3,
};

// 単一のプリセットファイルのメタデータを受信するレシーバー。
// ホストは、このインターフェースの様々なコールバックを定義し、プリセットパーサー関数が
// それらを呼び出します。
//
// このインターフェースはスレッドセーフではありません。
typedef struct clap_preset_discovery_metadata_receiver {
   void *receiver_data; // メタデータレシーバー用の予約ポインタ

   // ファイルからメタデータを読み取る際にエラーがある場合、エラー
   // メッセージと共にこれを呼び出す必要があります。
   // os_error: 該当する場合のオペレーティングシステムエラー。該当しない場合は
   // 非エラー値に設定します。例：unixとWindowsでは0。
   void(CLAP_ABI *on_error)(const struct clap_preset_discovery_metadata_receiver *receiver,
                            int32_t                                               os_error,
                            const char                                           *error_message);

   // これは、ファイル内の全てのプリセットに対して、下記の呼び出しで
   // プリセットメタデータが送信される前に呼び出される必要があります。
   //
   // プリセットファイルがプリセットコンテナの場合、nameとload_keyは必須です。
   // そうでなければ、それらはオプションです。
   //
   // load_keyは、プリセットロードプラグイン拡張を介してコンテナ内の
   // プリセットをロードするために使用されるマシンフレンドリーな文字列です。load_keyは、
   // プラグインが望む場合はサブパスにすることもできますが、データベースのプライマリキーや
   // バイナリオフセットのような他のユニークIDにすることもできます。その使用は完全にプラグイン次第です。
   //
   // 関数がfalseを返す場合、プロバイダーはレシーバーへのコールバックを停止する必要があります。
   bool(CLAP_ABI *begin_preset)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                const char                                           *name,
                                const char                                           *load_key);

   // このプリセットで使用できるプラグインIDを追加します。
   void(CLAP_ABI *add_plugin_id)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                 const clap_universal_plugin_id_t                     *plugin_id);

   // プリセットが属するサウンドパックを設定します。
   void(CLAP_ABI *set_soundpack_id)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                    const char *soundpack_id);

   // フラグを設定します。clap_preset_discovery_flagsを参照してください。
   // 設定されていない場合、場所から継承されます。
   void(CLAP_ABI *set_flags)(const struct clap_preset_discovery_metadata_receiver *receiver,
                             uint32_t                                              flags);

   // プリセットのクリエーター名を追加します。
   void(CLAP_ABI *add_creator)(const struct clap_preset_discovery_metadata_receiver *receiver,
                               const char                                           *creator);

   // プリセットの説明を設定します。
   void(CLAP_ABI *set_description)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                   const char *description);

   // プリセットの作成時刻と最終変更時刻を設定します。
   // 時刻のいずれかが不明な場合は、CLAP_TIMESTAMP_UNKNOWNに設定してください。
   // この関数が呼び出されない場合、インデクサーはファイルの作成および
   // 変更時刻を参照する場合があります。
   void(CLAP_ABI *set_timestamps)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                  clap_timestamp creation_time,
                                  clap_timestamp modification_time);

   // プリセットに機能を追加します。
   //
   // 機能文字列は任意で、それを理解し、内部の
   // 分類およびタグ付けシステムに再マップするのはインデクサーの仕事です。
   //
   // ただし、plugin-features.hの文字列はインデクサーによって理解されるべきで、
   // プリセットがaudio-effect、instrument、...のいずれかの結果になるかを決定するために
   // プラグインカテゴリの一つを提供できます。
   //
   // 例：
   // kick, drum, tom, snare, clap, cymbal, bass, lead, metalic, hardsync, crossmod, acid,
   // distorted, drone, pad, dirty, etc...
   void(CLAP_ABI *add_feature)(const struct clap_preset_discovery_metadata_receiver *receiver,
                               const char                                           *feature);

   // メタデータに追加情報を追加します。
   void(CLAP_ABI *add_extra_info)(const struct clap_preset_discovery_metadata_receiver *receiver,
                                  const char                                           *key,
                                  const char                                           *value);
} clap_preset_discovery_metadata_receiver_t;

typedef struct clap_preset_discovery_filetype {
   const char *name;
   const char *description; // オプション

   // '.'は文字列に含まれません。
   // 空またはNULLの場合、すべてのファイルがマッチする必要があります。
   const char *file_extension;
} clap_preset_discovery_filetype_t;

// プリセットを検索する場所を定義
typedef struct clap_preset_discovery_location {
   uint32_t    flags; // enum clap_preset_discovery_flagsを参照
   const char *name;  // この場所の名前
   uint32_t    kind;  // clap_preset_discovery_location_kindを参照

   // プリセットをクロールする実際の場所。
   // FILEカインドの場合、場所はディレクトリまたはファイルへのパスにできます。
   // PLUGINカインドの場合、場所はnullでなければなりません。
   const char *location;
} clap_preset_discovery_location_t;

// インストールされたサウンドパックを説明
typedef struct clap_preset_discovery_soundpack {
   uint32_t       flags;             // enum clap_preset_discovery_flagsを参照
   const char    *id;                // サウンドパック識別子
   const char    *name;              // このサウンドパックの名前
   const char    *description;       // オプション、サウンドパックの適度に短い説明
   const char    *homepage_url;      // オプション、パックのホームページへのURL
   const char    *vendor;            // オプション、サウンドパックのベンダー
   const char    *image_path;        // オプション、ディスク上の画像
   clap_timestamp release_timestamp; // リリース日、利用できない場合はCLAP_TIMESTAMP_UNKNOWN
} clap_preset_discovery_soundpack_t;

// プリセットプロバイダーを説明
typedef struct clap_preset_discovery_provider_descriptor {
   clap_version_t clap_version; // CLAP_VERSIONに初期化
   const char    *id;           // 良い識別子の選び方についてはplugin.hのアドバイスを参照
   const char    *name;         // 例: "Diva's preset provider"
   const char    *vendor;       // オプション、例: u-he
} clap_preset_discovery_provider_descriptor_t;

// このインターフェースはスレッドセーフではありません。
typedef struct clap_preset_discovery_provider {
   const clap_preset_discovery_provider_descriptor_t *desc;

   void *provider_data; // プロバイダー用の予約ポインタ

   // プリセットプロバイダーを初期化します。
   // すべての場所、ファイルタイプ、サウンドパックを宣言する必要があります。
   // 初期化が失敗した場合はfalseを返します。
   bool(CLAP_ABI *init)(const struct clap_preset_discovery_provider *provider);

   // プリセットプロバイダーを破棄します
   void(CLAP_ABI *destroy)(const struct clap_preset_discovery_provider *provider);

   // 指定されたファイルからメタデータを読み取り、メタデータレシーバーに渡します
   // 成功時にtrueを返します。
   bool(CLAP_ABI *get_metadata)(const struct clap_preset_discovery_provider     *provider,
                                uint32_t                                         location_kind,
                                const char                                      *location,
                                const clap_preset_discovery_metadata_receiver_t *metadata_receiver);

   // 拡張をクエリします。
   // 返されるポインタはプロバイダーが所有します。
   // provider->init()の前に呼び出すことは禁止されています。
   // provider->init()呼び出し内で、およびその後に呼び出すことができます。
   const void *(CLAP_ABI *get_extension)(const struct clap_preset_discovery_provider *provider,
                                         const char                                  *extension_id);
} clap_preset_discovery_provider_t;

// このインターフェースはスレッドセーフではありません
typedef struct clap_preset_discovery_indexer {
   clap_version_t clap_version; // CLAP_VERSIONに初期化
   const char    *name;         // 例: "Bitwig Studio"
   const char    *vendor;       // オプション、例: "Bitwig GmbH"
   const char    *url;          // オプション、例: "https://bitwig.com"
   const char *version; // オプション、例: "4.3"、バージョンのフォーマット方法についてはplugin.hのアドバイスを参照

   void *indexer_data; // インデクサー用の予約ポインタ

   // プリセットファイルタイプを宣言します。
   // この呼び出し中にプロバイダーにコールバックしないでください。
   // ファイルタイプが無効な場合はfalseを返します。
   bool(CLAP_ABI *declare_filetype)(const struct clap_preset_discovery_indexer *indexer,
                                    const clap_preset_discovery_filetype_t     *filetype);

   // プリセット場所を宣言します。
   // この呼び出し中にプロバイダーにコールバックしないでください。
   // 場所が無効な場合はfalseを返します。
   bool(CLAP_ABI *declare_location)(const struct clap_preset_discovery_indexer *indexer,
                                    const clap_preset_discovery_location_t     *location);

   // サウンドパックを宣言します。
   // この呼び出し中にプロバイダーにコールバックしないでください。
   // サウンドパックが無効な場合はfalseを返します。
   bool(CLAP_ABI *declare_soundpack)(const struct clap_preset_discovery_indexer *indexer,
                                     const clap_preset_discovery_soundpack_t    *soundpack);

   // 拡張をクエリします。
   // 返されるポインタはインデクサーが所有します。
   // provider->init()の前に呼び出すことは禁止されています。
   // provider->init()呼び出し内で、およびその後に呼び出すことができます。
   const void *(CLAP_ABI *get_extension)(const struct clap_preset_discovery_indexer *indexer,
                                         const char                                 *extension_id);
} clap_preset_discovery_indexer_t;

// このファクトリーのすべてのメソッドはスレッドセーフでなければなりません。
// バックグラウンドスレッド、さらにはバックグラウンドプロセスで
// プリセットインデックス化を実行することが推奨されます。
//
// ホストは、ファクトリーのコンテンツを変更する可能性のあるファイルシステムの変更を
// 検出するためにclap_plugin_invalidation_factoryを使用する場合があります。
typedef struct clap_preset_discovery_factory {
   // 利用可能なプリセットプロバイダー数を取得します。
   // [thread-safe]
   uint32_t(CLAP_ABI *count)(const struct clap_preset_discovery_factory *factory);

   // インデックスによってプリセットプロバイダーディスクリプタを取得します。
   // エラーの場合nullを返します。
   // ディスクリプタを解放してはいけません。
   // [thread-safe]
   const clap_preset_discovery_provider_descriptor_t *(CLAP_ABI *get_descriptor)(
      const struct clap_preset_discovery_factory *factory, uint32_t index);

   // IDによってプリセットプロバイダーを作成します。
   // 返されたポインタは、preset_provider->destroy(preset_provider);を呼び出すことで解放する必要があります。
   // プリセットプロバイダーは、createメソッドでインデクサーコールバックを使用することは許可されていません。
   // インデクサーがprovider->init()を呼び出す前に、インデクサーにコールバックすることは禁止されています。
   // エラーの場合nullを返します。
   // [thread-safe]
   const clap_preset_discovery_provider_t *(CLAP_ABI *create)(
      const struct clap_preset_discovery_factory *factory,
      const clap_preset_discovery_indexer_t      *indexer,
      const char                                 *provider_id);
} clap_preset_discovery_factory_t;

#ifdef __cplusplus
}
#endif