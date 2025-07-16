#pragma once

#include "../../plugin.h"
#include "../../events.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_TRIGGERS[] = "clap.triggers/1";

#ifdef __cplusplus
extern "C" {
#endif

/// @page トリガーイベント
///
/// この拡張により、プラグインはホストにトリガーのセットを公開できます。
///
/// トリガーの例：
/// - ノートとは独立したエンベロープをトリガーする
/// - サンプル&ホールドユニットをトリガーする（ボイス毎でも可能）

enum {
   // このトリガーはノートIDごとのオートメーションをサポートしますか？
   CLAP_TRIGGER_IS_AUTOMATABLE_PER_NOTE_ID = 1 << 0,

   // このトリガーはキーごとのオートメーションをサポートしますか？
   CLAP_TRIGGER_IS_AUTOMATABLE_PER_KEY = 1 << 1,

   // このトリガーはチャンネルごとのオートメーションをサポートしますか？
   CLAP_TRIGGER_IS_AUTOMATABLE_PER_CHANNEL = 1 << 2,

   // このトリガーはポートごとのオートメーションをサポートしますか？
   CLAP_TRIGGER_IS_AUTOMATABLE_PER_PORT = 1 << 3,
};
typedef uint32_t clap_trigger_info_flags;

// この拡張がまだドラフトであることを考慮すると、安定化するまでは
// イベントレジストリとその独自のイベント名前空間を使用します。
//
// #include <clap/ext/event-registry.h>
//
// uint16_t CLAP_EXT_TRIGGER_EVENT_SPACE_ID = UINT16_MAX;
// if (host_event_registry->query(host, CLAP_EXT_TRIGGERS, &CLAP_EXT_TRIGGER_EVENT_SPACE_ID)) {
//   /* トリガーイベントを使用できます */
// }
//
// /* 後で */
// clap_event_trigger ev;
// ev.header.space_id = CLAP_EXT_TRIGGER_EVENT_SPACE_ID;
// ev.header.type = CLAP_EVENT_TRIGGER;

enum { CLAP_EVENT_TRIGGER = 0 };

typedef struct clap_event_trigger {
   clap_event_header_t header;

   // 対象トリガー
   clap_id trigger_id; // @ref clap_trigger_info.id
   void   *cookie;     // @ref clap_trigger_info.cookie

   // 特定のnote_id、port、key、channelを対象とし、グローバルの場合は-1
   int32_t note_id;
   int16_t port_index;
   int16_t channel;
   int16_t key;
} clap_event_trigger_t;

/* これはトリガーを記述します */
typedef struct clap_trigger_info {
   // 安定したトリガー識別子、決して変更してはいけません。
   clap_id id;

   clap_trigger_info_flags flags;

   // clap_param_info.cookieと類似
   void *cookie;

   // 表示可能な名前
   char name[CLAP_NAME_SIZE];

   // トリガーを含むモジュールパス、例："sequencers/seq1"
   // '/'は区切り文字として使用され、ツリー状の構造を表示します。
   char module[CLAP_PATH_SIZE];
} clap_trigger_info_t;

typedef struct clap_plugin_triggers {
   // トリガー数を返します。
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // トリガーの情報をtrigger_infoにコピーし、成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *get_info)(const clap_plugin_t *plugin,
                            uint32_t             index,
                            clap_trigger_info_t *trigger_info);
} clap_plugin_triggers_t;

enum {
   // トリガー情報が変更された場合、このフラグを使用します：
   // - 名前の変更
   // - モジュールの変更
   // 新しい情報は即座に有効になります。
   CLAP_TRIGGER_RESCAN_INFO = 1 << 0,

   // ホストがトリガーについて知っているすべてを無効化します。
   // プラグインが非アクティブ化されている間のみ使用できます。
   // プラグインがアクティブ化されている場合は、clap_host->restart()を使用し、
   // ホストがclap_plugin->deactivate()を呼び出すまで変更を遅延させてください。
   //
   // 以下の場合はこのフラグを使用する必要があります：
   // - いくつかのトリガーが追加または削除された場合。
   // - いくつかのトリガーに重要な変更があった場合：
   //   - is_per_note (フラグ)
   //   - is_per_key (フラグ)
   //   - is_per_channel (フラグ)
   //   - is_per_port (フラグ)
   //   - cookie
   CLAP_TRIGGER_RESCAN_ALL = 1 << 1,
};
typedef uint32_t clap_trigger_rescan_flags;

enum {
   // トリガーへのすべての可能な参照をクリアします
   CLAP_TRIGGER_CLEAR_ALL = 1 << 0,

   // トリガーへのすべてのオートメーションをクリアします
   CLAP_TRIGGER_CLEAR_AUTOMATIONS = 1 << 1,
};
typedef uint32_t clap_trigger_clear_flags;

typedef struct clap_host_triggers {
   // フラグに従ってトリガーの完全なリストを再スキャンします。
   // [main-thread]
   void(CLAP_ABI *rescan)(const clap_host_t *host, clap_trigger_rescan_flags flags);

   // トリガーへの参照をクリアします。
   // [main-thread]
   void(CLAP_ABI *clear)(const clap_host_t       *host,
                         clap_id                  trigger_id,
                         clap_trigger_clear_flags flags);
} clap_host_triggers_t;

#ifdef __cplusplus
}
#endif