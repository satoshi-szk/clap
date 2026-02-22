#pragma once

#include "params.h"
#include "../color.h"

// この拡張は、ホストがプラグインにパラメータ上の小さな色ベースの表示を
// するよう指示することを可能にします。これは以下を示すために使用できます:
// - 物理コントローラーがパラメータにマップされている
// - パラメータが現在オートメーションを再生している
// - パラメータがオートメーションをオーバーライドしている
// - など...
//
// 色のセマンティクスはここではホストに依存し、目標は
// すべてのプラグイン間で一貫した体験を提供することです。

static CLAP_CONSTEXPR const char CLAP_EXT_PARAM_INDICATION[] = "clap.param-indication/4";

// 最新ドラフトは100%互換です。
// この互換IDは2026年に削除される可能性があります。
static CLAP_CONSTEXPR const char CLAP_EXT_PARAM_INDICATION_COMPAT[] = "clap.param-indication.draft/4";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // ホストはこのパラメータのオートメーションを持っていません
   CLAP_PARAM_INDICATION_AUTOMATION_NONE = 0,

   // ホストはこのパラメータのオートメーションを持っていますが、再生していません
   CLAP_PARAM_INDICATION_AUTOMATION_PRESENT = 1,

   // ホストはこのパラメータのオートメーションを再生しています
   CLAP_PARAM_INDICATION_AUTOMATION_PLAYING = 2,

   // ホストはこのパラメータでオートメーションを記録しています
   CLAP_PARAM_INDICATION_AUTOMATION_RECORDING = 3,

   // ホストはこのパラメータのオートメーションを再生すべきですが、ユーザーが
   // このパラメータの調整を開始し、オートメーション再生をオーバーライドしています
   CLAP_PARAM_INDICATION_AUTOMATION_OVERRIDING = 4,
};

typedef struct clap_plugin_param_indication {
   // マッピング表示を設定またはクリアします。
   //
   // has_mapping: パラメータは現在マッピングを持っていますか？
   // color: 設定されている場合、プラグインGUIでコントロールをハイライトするために使用する色
   // label: 設定されている場合、ハードウェアコントローラーを識別するノブの上に表示する小さな文字列
   // description: 設定されている場合、ツールチップで使用できる文字列で、現在のマッピングを記述します
   //
   // パラメータ表示はプラグインコンテキストに保存されるべきではなく、デフォルトでオフです。
   // [main-thread]
   void(CLAP_ABI *set_mapping)(const clap_plugin_t *plugin,
                               clap_id              param_id,
                               bool                 has_mapping,
                               const clap_color_t  *color,
                               const char          *label,
                               const char          *description);

   // オートメーション表示を設定またはクリアします。
   //
   // automation_state: 指定されたパラメータの現在のオートメーション状態
   // color: 設定されている場合、プラグインGUIでオートメーション表示を表示するために使用する色
   //
   // パラメータ表示はプラグインコンテキストに保存されるべきではなく、デフォルトでオフです。
   // [main-thread]
   void(CLAP_ABI *set_automation)(const clap_plugin_t *plugin,
                                  clap_id              param_id,
                                  uint32_t             automation_state,
                                  const clap_color_t  *color);
} clap_plugin_param_indication_t;

#ifdef __cplusplus
}
#endif