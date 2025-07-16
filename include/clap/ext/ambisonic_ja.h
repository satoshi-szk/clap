#pragma once

#include "../plugin.h"

// この拡張は、プラグインによって使用されるチャンネルマッピングを指定するために使用できます。
static CLAP_CONSTEXPR const char CLAP_EXT_AMBISONIC[] = "clap.ambisonic/3";

// 最新ドラフトは100%互換です。
// この互換IDは2026年に削除される可能性があります。
static CLAP_CONSTEXPR const char CLAP_EXT_AMBISONIC_COMPAT[] = "clap.ambisonic.draft/3";

static CLAP_CONSTEXPR const char CLAP_PORT_AMBISONIC[] = "ambisonic";

#ifdef __cplusplus
extern "C" {
#endif

enum clap_ambisonic_ordering {
   // FuMaチャンネル順序
   CLAP_AMBISONIC_ORDERING_FUMA = 0,

   // ACNチャンネル順序
   CLAP_AMBISONIC_ORDERING_ACN = 1,
};

enum clap_ambisonic_normalization {
   CLAP_AMBISONIC_NORMALIZATION_MAXN = 0,
   CLAP_AMBISONIC_NORMALIZATION_SN3D = 1,
   CLAP_AMBISONIC_NORMALIZATION_N3D = 2,
   CLAP_AMBISONIC_NORMALIZATION_SN2D = 3,
   CLAP_AMBISONIC_NORMALIZATION_N2D = 4,
};

typedef struct clap_ambisonic_config {
   uint32_t ordering;      // clap_ambisonic_orderingを参照
   uint32_t normalization; // clap_ambisonic_normalizationを参照
} clap_ambisonic_config_t;

typedef struct clap_plugin_ambisonic {
   // 与えられた設定がサポートされている場合trueを返します。
   // [main-thread]
   bool(CLAP_ABI *is_config_supported)(const clap_plugin_t           *plugin,
                                       const clap_ambisonic_config_t *config);

   // 成功時にtrueを返します
   // [main-thread]
   bool(CLAP_ABI *get_config)(const clap_plugin_t     *plugin,
                              bool                     is_input,
                              uint32_t                 port_index,
                              clap_ambisonic_config_t *config);

} clap_plugin_ambisonic_t;

typedef struct clap_host_ambisonic {
   // 情報が変更されたことをホストに通知します。
   // 情報はプラグインが非アクティブ化されている時のみ変更できます。
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_ambisonic_t;

#ifdef __cplusplus
}
#endif