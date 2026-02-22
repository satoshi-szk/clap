#pragma once

#include "../plugin.h"
#include "../string-sizes.h"

#ifdef __cplusplus
extern "C" {
#endif

static CLAP_CONSTEXPR const char CLAP_EXT_NOTE_NAME[] = "clap.note-name";

typedef struct clap_note_name {
   char    name[CLAP_NAME_SIZE];
   int16_t port;    // すべてのポートの場合は-1
   int16_t key;     // すべてのキーの場合は-1
   int16_t channel; // すべてのチャンネルの場合は-1
} clap_note_name_t;

typedef struct clap_plugin_note_name {
   // ノート名の数を返します
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // 成功時にtrueを返し、結果をnote_nameに格納します
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t *plugin, uint32_t index, clap_note_name_t *note_name);
} clap_plugin_note_name_t;

typedef struct clap_host_note_name {
   // ノート名が変更されたことをホストに通知します。
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_note_name_t;

#ifdef __cplusplus
}
#endif