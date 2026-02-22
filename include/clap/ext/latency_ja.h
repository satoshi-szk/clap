#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_LATENCY[] = "clap.latency";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_latency {
   // プラグインの遅延をサンプル単位で返します。
   // [main-thread & (being-activated | active)]
   uint32_t(CLAP_ABI *get)(const clap_plugin_t *plugin);
} clap_plugin_latency_t;

typedef struct clap_host_latency {
   // 遅延が変更されたことをホストに伝えます。
   // 遅延はプラグイン->activate中にのみ変更が許可されています。
   // プラグインがアクティブ化されている場合は、host->request_restart()を呼び出してください
   // [main-thread & being-activated]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_latency_t;

#ifdef __cplusplus
}
#endif