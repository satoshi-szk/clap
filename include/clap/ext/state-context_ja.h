#pragma once

#include "../plugin.h"
#include "../stream.h"

/// @page state-context extension
/// @brief 拡張状態処理
///
/// この拡張は、ホストがコンテキストに応じて異なるセマンティクスで
/// プラグイン状態を保存およびロードすることを可能にします。
///
/// 簡潔に言うと、プリセットをロードしたりデバイスを複製したりするとき、
/// プラグインは状態を部分的にロードし、限られたリソースや外部ハードウェア
/// リソースへの固定接続の処理など、特定のことを異なって初期化したい場合があります。
///
/// 保存および読み込み操作は異なるコンテキストを持つ場合があります。
/// 以下の3つの操作はすべて等価であるべきです:
/// 1. clap_plugin_state_context.load(clap_plugin_state.save(), CLAP_STATE_CONTEXT_FOR_PRESET)
/// 2. clap_plugin_state.load(clap_plugin_state_context.save(CLAP_STATE_CONTEXT_FOR_PRESET))
/// 3. clap_plugin_state_context.load(
///        clap_plugin_state_context.save(CLAP_STATE_CONTEXT_FOR_PRESET),
///        CLAP_STATE_CONTEXT_FOR_PRESET)
///
/// 疑問がある場合は、clap_plugin_stateにフォールバックしてください。
///
/// プラグインがCLAP_EXT_STATE_CONTEXTを実装する場合、
/// CLAP_EXT_STATEも実装することが必須です。
///
/// clap_plugin_state.{save,load}()と等価なコンテキストは未指定です

#ifdef __cplusplus
extern "C" {
#endif

static CLAP_CONSTEXPR const char CLAP_EXT_STATE_CONTEXT[] = "clap.state-context/2";

enum clap_plugin_state_context_type {
   // プリセットとして状態を保存およびロードするのに適している
   CLAP_STATE_CONTEXT_FOR_PRESET = 1,

  // プラグインインスタンスを複製するのに適している
   CLAP_STATE_CONTEXT_FOR_DUPLICATE = 2,

   // プロジェクト/楽曲内で状態を保存およびロードするのに適している
   CLAP_STATE_CONTEXT_FOR_PROJECT = 3,
};

typedef struct clap_plugin_state_context {
   // context_typeに従ってプラグイン状態をストリームに保存します。
   // 状態が正しく保存された場合trueを返します。
   //
   // 結果はclap_plugin_state.load()とclap_plugin_state_context.load()の
   // 両方でロードされる可能性があることに注意してください。
   // [main-thread]
   bool(CLAP_ABI *save)(const clap_plugin_t  *plugin,
                        const clap_ostream_t *stream,
                        uint32_t              context_type);

   // context_typeに従ってストリームからプラグイン状態をロードします。
   // 状態が正しく復元された場合trueを返します。
   //
   // 状態がclap_plugin_state.save()またはclap_plugin_state_context.save()で
   // 異なるcontext_typeで保存された可能性があることに注意してください。
   // [main-thread]
   bool(CLAP_ABI *load)(const clap_plugin_t  *plugin,
                        const clap_istream_t *stream,
                        uint32_t              context_type);
} clap_plugin_state_context_t;

#ifdef __cplusplus
}
#endif