#pragma once

#include "../../id.h"
#include "../../universal-plugin-id.h"
#include "../../stream.h"
#include "../../version.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_state_converter_descriptor {
   clap_version_t clap_version;

   clap_universal_plugin_id_t src_plugin_id;
   clap_universal_plugin_id_t dst_plugin_id;

   const char *id;          // 例: "com.u-he.diva-converter", 必須
   const char *name;        // 例: "Diva Converter", 必須
   const char *vendor;      // 例: "u-he"
   const char *version;     // 例: 1.1.5
   const char *description; // 例: "Official state converter for u-he Diva."
} clap_plugin_state_converter_descriptor_t;

// このインターフェースは、ホストがプラグインの状態とそのオートメーション
// ポイントを新しいプラグインに変換するメカニズムを提供します。
//
// これは、あるプラグインABIから別のプラグインABIに変換するのに便利です。
// これは、アップグレードパスを提供するのにも便利です：EQバージョン1からEQバージョン2へ。
// これは、もはやメンテナンスされていないプラグインの状態を
// 類似した別のプラグインに変換するためにも使用できます。
typedef struct clap_plugin_state_converter {
   const clap_plugin_state_converter_descriptor_t *desc;

   void *converter_data;

   // コンバーターを破棄します。
   void (*destroy)(struct clap_plugin_state_converter *converter);

   // 入力状態を、対象プラグインで使用可能な状態に変換します。
   //
   // error_bufferは、失敗時にヌル終端エラーメッセージを格納するための
   // error_buffer_sizeバイトのプレースホルダーで、ユーザーに表示できます。
   //
   // 成功時にtrueを返します。
   // [thread-safe]
   bool (*convert_state)(struct clap_plugin_state_converter *converter,
                         const clap_istream_t               *src,
                         const clap_ostream_t               *dst,
                         char                               *error_buffer,
                         size_t                              error_buffer_size);

   // 正規化された値を変換します。
   // 成功時にtrueを返します。
   // [thread-safe]
   bool (*convert_normalized_value)(struct clap_plugin_state_converter *converter,
                                    clap_id                             src_param_id,
                                    double                              src_normalized_value,
                                    clap_id                            *dst_param_id,
                                    double                             *dst_normalized_value);

   // プレーン値を変換します。
   // 成功時にtrueを返します。
   // [thread-safe]
   bool (*convert_plain_value)(struct clap_plugin_state_converter *converter,
                               clap_id                             src_param_id,
                               double                              src_plain_value,
                               clap_id                            *dst_param_id,
                               double                             *dst_plain_value);
} clap_plugin_state_converter_t;

// ファクトリー識別子
static CLAP_CONSTEXPR const char CLAP_PLUGIN_STATE_CONVERTER_FACTORY_ID[] =
   "clap.plugin-state-converter-factory/1";

// 現在のDSOで利用可能なすべてのプラグイン状態コンバーターをリストします。
typedef struct clap_plugin_state_converter_factory {
   // コンバーター数を取得します。
   // [thread-safe]
   uint32_t (*count)(const struct clap_plugin_state_converter_factory *factory);

   // インデックスによってプラグイン状態コンバーターディスクリプタを取得します。
   // エラーの場合nullを返します。
   // ディスクリプタを解放してはいけません。
   // [thread-safe]
   const clap_plugin_state_converter_descriptor_t *(*get_descriptor)(
      const struct clap_plugin_state_converter_factory *factory, uint32_t index);

   // converter_idによってプラグイン状態コンバーターを作成します。
   // 返されたポインタは、converter->destroy(converter);を呼び出すことで解放する必要があります。
   // エラーの場合nullを返します。
   // [thread-safe]
   clap_plugin_state_converter_t *(*create)(
      const struct clap_plugin_state_converter_factory *factory, const char *converter_id);
} clap_plugin_state_converter_factory_t;

#ifdef __cplusplus
}
#endif