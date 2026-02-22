#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_RENDER[] = "clap.render";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // デフォルト設定、「リアルタイム」処理用
   CLAP_RENDER_REALTIME = 0,

   // リアルタイムプレッシャーなしでの処理用
   // プラグインは、より高い音質のためにより高価なアルゴリズムを使用する可能性があります。
   CLAP_RENDER_OFFLINE = 1,
};
typedef int32_t clap_plugin_render_mode;

// render拡張は、プラグインが処理に「リアルタイム」プレッシャーがあるかどうかを
// 知らせるために使用されます。
//
// この情報がレンダリングコードに影響しない場合は、
// この拡張を実装しないでください。
typedef struct clap_plugin_render {
   // プラグインがリアルタイムで処理するハード要件を持っている場合trueを返します。
   // これは、ハードウェアデバイスへのプロキシとして動作するプラグインに特に有用です。
   // [main-thread]
   bool(CLAP_ABI *has_hard_realtime_requirement)(const clap_plugin_t *plugin);

   // レンダリングモードが適用できた場合trueを返します。
   // [main-thread]
   bool(CLAP_ABI *set)(const clap_plugin_t *plugin, clap_plugin_render_mode mode);
} clap_plugin_render_t;

#ifdef __cplusplus
}
#endif