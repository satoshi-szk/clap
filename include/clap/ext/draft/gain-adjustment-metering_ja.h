#pragma once

#include "../../plugin.h"

// この拡張により、プラグインは現在のゲイン調整
// （通常、ゲインリダクション）をホストに報告できます。

static CLAP_CONSTEXPR const char CLAP_EXT_GAIN_ADJUSTMENT_METERING[] = "clap.gain-adjustment-metering/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_gain_adjustment_metering {
  // 現在のゲイン調整をdBで返します。この値は
  // ホストメーターやツールチップなどでの情報表示を目的としています。
  // 返される値は、最新に処理されたブロックの最後のサンプルに対して
  // プラグインが適用したゲイン調整を表します。
  //
  // 返される値はdB単位です。ゼロは、プラグインがゲインリダクションを
  // 適用していない、または処理していないことを意味します。負の値は、
  // コンプレッサーやリミッターのように、プラグインがゲインリダクションを
  // 適用していることを意味します。正の値は、エキスパンダーのように、
  // プラグインがゲインを追加していることを意味します。この値は、
  // メイクアップゲインやその他の調整の前に、プラグインによって適用される
  // 動的なゲインリダクションまたはエクスパンションを表します。
  // すべてのオーディオチャンネルに対して単一の値が返されます。
  //
  // [audio-thread]
  double(CLAP_ABI *get)(const clap_plugin_t *plugin);
} clap_plugin_gain_adjustment_metering_t;

#ifdef __cplusplus
}
#endif