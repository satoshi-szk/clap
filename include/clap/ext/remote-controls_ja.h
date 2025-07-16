#pragma once

#include "../plugin.h"
#include "../string-sizes.h"

// この拡張は、プラグインがパラメータをハードウェアコントローラーに
// マッピングする構造化された方法を提供することを可能にします。
//
// これは、セクションごとに整理されたリモートコントロールページのセットを
// 提供することによって行われます。
// ページには最大8つのコントロールが含まれ、param_idを使用してパラメータを参照します。
//
// |`- [section:main]
// |    `- [name:main] performance controls
// |`- [section:osc]
// |   |`- [name:osc1] osc1 page
// |   |`- [name:osc2] osc2 page
// |   |`- [name:osc-sync] osc sync page
// |    `- [name:osc-noise] osc noise page
// |`- [section:filter]
// |   |`- [name:flt1] filter 1 page
// |    `- [name:flt2] filter 2 page
// |`- [section:env]
// |   |`- [name:env1] env1 page
// |    `- [name:env2] env2 page
// |`- [section:lfo]
// |   |`- [name:lfo1] env1 page
// |    `- [name:lfo2] env2 page
//  `- etc...
//
// 可能なワークフローの1つは、セクションに対応するボタンのセットを持つことです。
// そのボタンを一度押すと、セクションの最初のページに移動します。
// もう一度押すと、セクションのページを循環します。

static CLAP_CONSTEXPR const char CLAP_EXT_REMOTE_CONTROLS[] = "clap.remote-controls/2";

// 最新ドラフトは100%互換です
// この互換IDは2026年に削除される可能性があります。
static CLAP_CONSTEXPR const char CLAP_EXT_REMOTE_CONTROLS_COMPAT[] = "clap.remote-controls.draft/2";

#ifdef __cplusplus
extern "C" {
#endif

enum { CLAP_REMOTE_CONTROLS_COUNT = 8 };

typedef struct clap_remote_controls_page {
   char    section_name[CLAP_NAME_SIZE];
   clap_id page_id;
   char    page_name[CLAP_NAME_SIZE];
   clap_id param_ids[CLAP_REMOTE_CONTROLS_COUNT];

   // これは、デバイスページとプリセットページを分離するために使用されます。
   // trueの場合、このページはこのプリセットに固有です。
   bool is_for_preset;
} clap_remote_controls_page_t;

typedef struct clap_plugin_remote_controls {
   // ページ数を返します。
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // インデックスでページを取得します。
   // 成功時にtrueを返し、結果をpageに格納します。
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t         *plugin,
                       uint32_t                     page_index,
                       clap_remote_controls_page_t *page);
} clap_plugin_remote_controls_t;

typedef struct clap_host_remote_controls {
   // リモートコントロールが変更されたことをホストに通知します。
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);

   // ユーザーが現在プラグインのGUIで編集している内容に対応するため、
   // ホストにページを提案します。
   // [main-thread]
   void(CLAP_ABI *suggest_page)(const clap_host_t *host, clap_id page_id);
} clap_host_remote_controls_t;

#ifdef __cplusplus
}
#endif