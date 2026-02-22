#pragma once

#include "private/macros.h"
#include "host.h"
#include "process.h"
#include "plugin-features.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_descriptor {
   clap_version_t clap_version; // CLAP_VERSIONで初期化

   // 必須フィールドは設定され、空白であってはいけません。
   // その他のフィールドはnullまたは空白でも構いませんが、空白にする方が安全です。
   //
   // idとversionに関するいくつかの指針
   // - idはプラグインに一意であるべき任意の文字列です。
   //   逆URIの使用をお勧めします。例: "com.u-he.diva"
   // - versionはプラグインを説明する任意の文字列です。
   //   ホストが理解し、2つの異なるバージョン文字列を比較できることが有用です。
   //   そのため、ほとんどのホストが理解する可能性の高い正規表現のような表現は次のとおりです:
   //   MAJOR(.MINOR(.REVISION)?)?( (Alpha|Beta) XREV)?
   const char *id;          // 例: "com.u-he.diva"、必須
   const char *name;        // 例: "Diva"、必須
   const char *vendor;      // 例: "u-he"
   const char *url;         // 例: "https://u-he.com/products/diva/"
   const char *manual_url;  // 例: "https://dl.u-he.com/manuals/plugins/diva/Diva-user-guide.pdf"
   const char *support_url; // 例: "https://u-he.com/support/"
   const char *version;     // 例: "1.4.4"
   const char *description; // 例: "The spirit of analogue"

   // 任意のキーワードリスト。
   // ホストインデクサーによってマッチされ、プラグインの分類に使用できます。
   // ポインター配列はnullで終端される必要があります。
   // 標準機能についてはplugin-features.hを参照してください。
   const char *const *features;
} clap_plugin_descriptor_t;

typedef struct clap_plugin {
   const clap_plugin_descriptor_t *desc;

   void *plugin_data; // プラグイン用の予約ポインター

   // プラグインを作成した後に呼び出される必要があります。
   // initがfalseを返した場合、ホストはプラグインインスタンスを破棄する必要があります。
   // initがtrueを返した場合、プラグインは初期化され、非アクティブ状態になります。
   // `plugin-factory::create_plugin`とは異なり、initではホストとホスト拡張への
   // 完全なアクセスがあるため、clap関連のセットアップ活動は
   // create_pluginではなくここで行うべきです。
   // [main-thread]
   bool(CLAP_ABI *init)(const struct clap_plugin *plugin);

   // プラグインとそのリソースを解放します。
   // この呼び出しの前にプラグインを非アクティブ化する必要があります。
   // [main-thread & !active]
   void(CLAP_ABI *destroy)(const struct clap_plugin *plugin);

   // プラグインをアクティブ化および非アクティブ化します。
   // この呼び出しでプラグインはメモリを割り当て、プロセス呼び出しに
   // 必要なすべてを準備できます。プロセスのサンプルレートは一定で、
   // プロセスのフレーム数は[1, INT32_MAX]で制限される[min, max]範囲に含まれます。
   // この呼び出しでプラグインは[being-activated]とマークされた
   // ホスト提供メソッドを呼び出すことができます。
   // 一度アクティブ化されると、非アクティブ化まで遅延とポート構成は
   // 一定に保たれる必要があります。
   // 成功時にtrueを返します。
   // [main-thread & !active]
   bool(CLAP_ABI *activate)(const struct clap_plugin *plugin,
                            double                    sample_rate,
                            uint32_t                  min_frames_count,
                            uint32_t                  max_frames_count);
   // [main-thread & active]
   void(CLAP_ABI *deactivate)(const struct clap_plugin *plugin);

   // 処理前にstart processingを呼び出します。
   // 成功時にtrueを返します。
   // [audio-thread & active & !processing]
   bool(CLAP_ABI *start_processing)(const struct clap_plugin *plugin);

   // プラグインをスリープ状態に送る前にstop processingを呼び出します。
   // [audio-thread & active & processing]
   void(CLAP_ABI *stop_processing)(const struct clap_plugin *plugin);

   // - すべてのバッファをクリアし、処理状態（フィルター、オシレーター、
   //   エンベロープ、lfoなど）の完全なリセットを実行し、すべてのボイスを停止します。
   // - パラメータの値は変更されません。
   // - clap_process.steady_timeは後方にジャンプする可能性があります。
   //
   // [audio-thread & active]
   void(CLAP_ABI *reset)(const struct clap_plugin *plugin);

   // オーディオ、イベントなどを処理します。
   // clap_process_tとその入れ子になった属性からのすべてのポインターは、
   // process()が戻るまで有効です。
   // [audio-thread & active & processing]
   clap_process_status(CLAP_ABI *process)(const struct clap_plugin *plugin,
                                          const clap_process_t     *process);

   // 拡張を照会します。
   // 返されたポインターはプラグインが所有します。
   // plugin->init()の前に呼び出すことは禁止されています。
   // plugin->init()呼び出し内およびその後で呼び出すことができます。
   // [thread-safe]
   const void *(CLAP_ABI *get_extension)(const struct clap_plugin *plugin, const char *id);

   // 以前の次の呼び出しに応答して、ホストによってメインスレッドで呼び出されます:
   //   host->request_callback(host);
   // [main-thread]
   void(CLAP_ABI *on_main_thread)(const struct clap_plugin *plugin);
} clap_plugin_t;

#ifdef __cplusplus
}
#endif