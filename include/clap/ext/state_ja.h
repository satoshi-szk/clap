#pragma once

#include "../plugin.h"
#include "../stream.h"

/// @page State
/// @brief 状態管理
///
/// プラグインはこの拡張を実装して、パラメータ値と非パラメータ状態の
/// 両方を保存および復元できます。これは、プロジェクトリロード間での
/// プラグインの状態の永続化、プラグインインスタンスの複製とコピー、
/// およびホスト側のプリセット管理に使用されます。
///
/// 保存/ロード操作がプラグインインスタンスの複製、プラグインプリセットの
/// 保存/ロード、またはプロジェクトの保存/ロード中のものかを知る必要がある場合は、
/// CLAP_EXT_STATEに加えてCLAP_EXT_STATE_CONTEXTの実装を検討してください。

static CLAP_CONSTEXPR const char CLAP_EXT_STATE[] = "clap.state";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_state {
   // プラグイン状態をストリームに保存します。
   // 状態が正しく保存された場合trueを返します。
   // [main-thread]
   bool(CLAP_ABI *save)(const clap_plugin_t *plugin, const clap_ostream_t *stream);

   // ストリームからプラグイン状態をロードします。
   // 状態が正しく復元された場合trueを返します。
   // [main-thread]
   bool(CLAP_ABI *load)(const clap_plugin_t *plugin, const clap_istream_t *stream);
} clap_plugin_state_t;

typedef struct clap_host_state {
   // プラグイン状態が変更され、再度保存されるべきであることをホストに伝えます。
   // パラメータ値が変更された場合、状態がダーティであることは暗黙的です。
   // [main-thread]
   void(CLAP_ABI *mark_dirty)(const clap_host_t *host);
} clap_host_state_t;

#ifdef __cplusplus
}
#endif