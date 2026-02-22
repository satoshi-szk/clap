#pragma once

#include "../plugin.h"
#include "../string-sizes.h"

/// @page Note Ports
///
/// この拡張は、プラグインが現在のノートポートを記述する方法を提供します。
/// プラグインがこの拡張を実装しない場合、ノートの入力または出力を持ちません。
/// プラグインは、非アクティブ化されている間のみノートポート構成の変更が許可されています。

static CLAP_CONSTEXPR const char CLAP_EXT_NOTE_PORTS[] = "clap.note-ports";

#ifdef __cplusplus
extern "C" {
#endif

enum clap_note_dialect {
   // clap_event_noteとclap_event_note_expressionを使用します。
   CLAP_NOTE_DIALECT_CLAP = 1 << 0,

   // clap_event_midiを使用し、ポリフォニックエクスプレッションはありません
   CLAP_NOTE_DIALECT_MIDI = 1 << 1,

   // clap_event_midiを使用し、ポリフォニックエクスプレッション（MPE）を使用します
   CLAP_NOTE_DIALECT_MIDI_MPE = 1 << 2,

   // clap_event_midi2を使用します
   CLAP_NOTE_DIALECT_MIDI2 = 1 << 3,
};

typedef struct clap_note_port_info {
   // idはポートを識別し、安定している必要があります。
   // idは入力ポートと出力ポート間で重複する可能性があります。
   clap_id  id;
   uint32_t supported_dialects;   // ビットフィールド、clap_note_dialectを参照
   uint32_t preferred_dialect;    // clap_note_dialectの1つの値
   char     name[CLAP_NAME_SIZE]; // 表示可能な名前、i18n？
} clap_note_port_info_t;

// ノートポートスキャンは、プラグインが非アクティブ化されている間に実行される必要があります。
typedef struct clap_plugin_note_ports {
   // 入力または出力のいずれかのポート数。
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin, bool is_input);

   // ノートポートに関する情報を取得します。
   // 成功時にtrueを返し、結果をinfoに格納します。
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t   *plugin,
                       uint32_t               index,
                       bool                   is_input,
                       clap_note_port_info_t *info);
} clap_plugin_note_ports_t;

enum {
   // ポートが変更されました。ホストはポートの完全なスキャンを実行する必要があります。
   // このフラグは、プラグインがアクティブでない場合にのみ使用できます。
   // プラグインがアクティブな場合は、host->request_restart()を呼び出し、
   // ホストがdeactivate()を呼び出したときにrescan()を呼び出してください
   CLAP_NOTE_PORTS_RESCAN_ALL = 1 << 0,

   // ポート名が変更されました。ホストはすぐにスキャンできます。
   CLAP_NOTE_PORTS_RESCAN_NAMES = 1 << 1,
};

typedef struct clap_host_note_ports {
   // ホストがサポートする方言を照会します
   // [main-thread]
   uint32_t(CLAP_ABI *supported_dialects)(const clap_host_t *host);

   // フラグに従ってノートポートの完全なリストを再スキャンします。
   // [main-thread]
   void(CLAP_ABI *rescan)(const clap_host_t *host, uint32_t flags);
} clap_host_note_ports_t;

#ifdef __cplusplus
}
#endif