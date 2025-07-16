#pragma once

#include "../plugin.h"

// この拡張は、プラグインによって使用されるチャンネルマッピングを指定するために使用できます。
//
// すべてのプラグインインスタンス間で一貫したサラウンド機能を持つために、
// 以下のワークフローが提案されています:
// 1. プラグインがホストの推奨チャンネルマッピングを照会し、
//    それに合わせて構成を調整します。
// 2. ホストはプラグインが実際にどのように構成されているかをチェックし、それを尊重します。
//
// ホストがプロジェクトのサラウンドセットアップを変更することを決定した場合:
// 1. プラグインを非アクティブ化
// 2. ホストがclap_plugin_surround->changed()を呼び出し
// 3. プラグインがclap_host_surround->get_preferred_channel_map()を呼び出し
// 4. プラグインが最終的にclap_host_surround->changed()を呼び出し
// 5. 変更された場合、ホストがclap_plugin_surround->get_channel_map()を呼び出し
// 6. ホストがプラグインをアクティブ化し、オーディオ処理を開始できます
//
// プラグインがサラウンドセットアップを変更したい場合:
// 1. プラグインがアクティブな場合はhost->request_restart()を呼び出し
// 2. 非アクティブ化されたら、プラグインがclap_host_surround->changed()を呼び出し
// 3. ホストがclap_plugin_surround->get_channel_map()を呼び出し
// 4. ホストがプラグインをアクティブ化し、オーディオ処理を開始できます

static CLAP_CONSTEXPR const char CLAP_EXT_SURROUND[] = "clap.surround/4";

// 最新ドラフトは100%互換です。
// この互換IDは2026年に削除される可能性があります。
static CLAP_CONSTEXPR const char CLAP_EXT_SURROUND_COMPAT[] = "clap.surround.draft/4";

static CLAP_CONSTEXPR const char CLAP_PORT_SURROUND[] = "surround";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_SURROUND_FL = 0,   // フロントレフト
   CLAP_SURROUND_FR = 1,   // フロントライト
   CLAP_SURROUND_FC = 2,   // フロントセンター
   CLAP_SURROUND_LFE = 3,  // 低周波数
   CLAP_SURROUND_BL = 4,   // バック（リア）レフト
   CLAP_SURROUND_BR = 5,   // バック（リア）ライト
   CLAP_SURROUND_FLC = 6,  // フロントレフトオブセンター
   CLAP_SURROUND_FRC = 7,  // フロントライトオブセンター
   CLAP_SURROUND_BC = 8,   // バック（リア）センター
   CLAP_SURROUND_SL = 9,   // サイドレフト
   CLAP_SURROUND_SR = 10,  // サイドライト
   CLAP_SURROUND_TC = 11,  // トップ（ハイト）センター
   CLAP_SURROUND_TFL = 12, // トップ（ハイト）フロントレフト
   CLAP_SURROUND_TFC = 13, // トップ（ハイト）フロントセンター
   CLAP_SURROUND_TFR = 14, // トップ（ハイト）フロントライト
   CLAP_SURROUND_TBL = 15, // トップ（ハイト）バック（リア）レフト
   CLAP_SURROUND_TBC = 16, // トップ（ハイト）バック（リア）センター
   CLAP_SURROUND_TBR = 17, // トップ（ハイト）バック（リア）ライト
   CLAP_SURROUND_TSL = 18, // トップ（ハイト）サイドレフト
   CLAP_SURROUND_TSR = 19, // トップ（ハイト）サイドライト
};

typedef struct clap_plugin_surround {
   // 指定されたチャンネルマスクがサポートされているかチェックします。
   // チャンネルマスクはビットマスクです。例:
   //   (1 << CLAP_SURROUND_FL) | (1 << CLAP_SURROUND_FR) | ...
   // [main-thread]
   bool(CLAP_ABI *is_channel_mask_supported)(const clap_plugin_t *plugin, uint64_t channel_mask);

   // 各チャンネルのサラウンド識別子をchannel_map配列に格納します。
   // channel_mapに格納された要素数を返します。
   // channel_map_capacityは、指定されたポートのチャンネル数以上である必要があります。
   // [main-thread]
   uint32_t(CLAP_ABI *get_channel_map)(const clap_plugin_t *plugin,
                                       bool                 is_input,
                                       uint32_t             port_index,
                                       uint8_t             *channel_map,
                                       uint32_t             channel_map_capacity);
} clap_plugin_surround_t;

typedef struct clap_host_surround {
   // チャンネルマップが変更されたことをホストに通知します。
   // チャンネルマップは、プラグインが非アクティブ化されている時のみ変更できます。
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_surround_t;

#ifdef __cplusplus
}
#endif