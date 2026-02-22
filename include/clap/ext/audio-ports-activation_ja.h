#pragma once

#include "../plugin.h"

/// @page Audio Ports Activation
///
/// この拡張は、ホストがオーディオポートをアクティブ化および非アクティブ化する方法を提供します。
/// ポートの非アクティブ化は以下の利点を提供します:
/// - プラグインは特定の入力が存在しないことを事前に知ることができ、
///   最適化された計算パスを選択できます。
/// - プラグインは出力がホストによって消費されないことを知り、
///   それを計算する必要がありません。
///
/// オーディオポートは、can_activate_while_processing()がtrueを返さない限り、
/// プラグインが非アクティブ化されている時のみアクティブ化または非アクティブ化できます。
///
/// オーディオポートが非アクティブ化されていても、オーディオバッファは引き続き提供される必要があります。
/// そのような場合、それらは0（またはコンテキストでの中性値）で埋められ、
/// constant_maskが設定される必要があります。
///
/// オーディオポートは、プラグインインスタンス作成後、最初はアクティブ状態にあります。
/// オーディオポート状態はプラグイン状態に保存されないため、ホストは
/// プラグインインスタンス作成後にオーディオポート状態を復元する必要があります。
///
/// オーディオポート状態は、clap_plugin_audio_ports_config.select()と
/// clap_host_audio_ports.rescan(CLAP_AUDIO_PORTS_RESCAN_LIST)によって無効化されます。

static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS_ACTIVATION[] =
   "clap.audio-ports-activation/2";

// 最新ドラフトは100%互換です。
// この互換IDは2026年に削除される可能性があります。
static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS_ACTIVATION_COMPAT[] =
   "clap.audio-ports-activation/draft-2";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_audio_ports_activation {
   // プラグインが処理中のアクティブ化/非アクティブ化をサポートしている場合trueを返します。
   // [main-thread]
   bool(CLAP_ABI *can_activate_while_processing)(const clap_plugin_t *plugin);

   // 指定されたポートをアクティブ化します。
   //
   // can_activate_while_processing()がtrueを返す場合のみ、
   // オーディオスレッドでアクティブ化と非アクティブ化が可能です。
   //
   // sample_sizeは、ホストが32ビットオーディオバッファまたは64ビットオーディオバッファを
   // 提供するかを示します。
   // 可能な値は: 32、64、または未指定の場合は0です。
   //
   // 失敗または無効なパラメータの場合はfalseを返します
   // [active ? audio-thread : main-thread]
   bool(CLAP_ABI *set_active)(const clap_plugin_t *plugin,
                              bool                 is_input,
                              uint32_t             port_index,
                              bool                 is_active,
                              uint32_t             sample_size);
} clap_plugin_audio_ports_activation_t;

#ifdef __cplusplus
}
#endif