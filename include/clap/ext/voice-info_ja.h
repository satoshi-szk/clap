#pragma once

#include "../plugin.h"

// この拡張は、シンセサイザーが持つボイス数を示します。
// これは、ホストがポリフォニックモジュレーションを実行する際に有用です。
// なぜなら、ホストは独自のボイス管理を必要とし、プラグインが行っていることに
// 従おうとするべきだからです:
// - ホストのボイスプールをプラグインが持つものと一貫させる
// - プラグインがモノの場合、ホストのボイス管理をモノに切り替える

static CLAP_CONSTEXPR const char CLAP_EXT_VOICE_INFO[] = "clap.voice-info";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // ホストが重複するNOTE_ONイベントを送信することを許可します。
   // プラグインは、それらを区別するためにnote_idに依存します。
   CLAP_VOICE_INFO_SUPPORTS_OVERLAPPING_NOTES = 1 << 0,
};

typedef struct clap_voice_info {
   // voice_countは、パッチが使用できる現在のボイス数です
   // voice_capacityは、割り当てられたボイス数です
   // voice_countは、アクティブなボイス数と混同されるべきではありません。
   //
   // 1 <= voice_count <= voice_capacity
   //
   // 例えば、シンセは8ボイスの容量を持つことができますが、
   // 4ボイスのみを使用するように構成されています: {count: 4, capacity: 8}。
   //
   // voice_countが1の場合、シンセはモノで動作しており、
   // ホストはグローバルモジュレーションマッピングのみを使用することを決定できます。
   uint32_t voice_count;
   uint32_t voice_capacity;

   uint64_t flags;
} clap_voice_info_t;

typedef struct clap_plugin_voice_info {
   // ボイス情報を取得し、成功時にtrueを返します
   // [main-thread && active]
   bool(CLAP_ABI *get)(const clap_plugin_t *plugin, clap_voice_info_t *info);
} clap_plugin_voice_info_t;

typedef struct clap_host_voice_info {
   // ボイス情報が変更されたことをホストに通知します
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_voice_info_t;

#ifdef __cplusplus
}
#endif