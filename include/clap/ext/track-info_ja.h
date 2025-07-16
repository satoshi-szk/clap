#pragma once

#include "../plugin.h"
#include "../color.h"
#include "../string-sizes.h"

// この拡張は、プラグインが所属するトラックに関する情報を照会することを可能にします。
// これは、いくつかのパラメータ（ミックス、ドライ、ウェット）を初期化し、
// オーディオポートタイプとチャンネル数に関して適切な構成を選択するために、
// プラグインが作成されるときに有用です。

static CLAP_CONSTEXPR const char CLAP_EXT_TRACK_INFO[] = "clap.track-info/1";

// 最新ドラフトは100%互換です。
// この互換IDは2026年に削除される可能性があります。
static CLAP_CONSTEXPR const char CLAP_EXT_TRACK_INFO_COMPAT[] = "clap.track-info.draft/1";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_TRACK_INFO_HAS_TRACK_NAME = (1 << 0),
   CLAP_TRACK_INFO_HAS_TRACK_COLOR = (1 << 1),
   CLAP_TRACK_INFO_HAS_AUDIO_CHANNEL = (1 << 2),

   // このプラグインはリターントラック上にあり、ウェット100%で初期化します
   CLAP_TRACK_INFO_IS_FOR_RETURN_TRACK = (1 << 3),

   // このプラグインはバストラック上にあり、バス処理に適した設定で初期化します
   CLAP_TRACK_INFO_IS_FOR_BUS = (1 << 4),

   // このプラグインはマスター上にあり、チャンネル処理に適した設定で初期化します
   CLAP_TRACK_INFO_IS_FOR_MASTER = (1 << 5),
};

typedef struct clap_track_info {
   uint64_t flags; // 上記のフラグを参照

   // トラック名、フラグにCLAP_TRACK_INFO_HAS_TRACK_NAMEが含まれている場合利用可能
   char name[CLAP_NAME_SIZE];

   // トラック色、フラグにCLAP_TRACK_INFO_HAS_TRACK_COLORが含まれている場合利用可能
   clap_color_t color;

   // フラグにCLAP_TRACK_INFO_HAS_AUDIO_CHANNELが含まれている場合利用可能
   // チャンネル数とポートタイプの使用方法については、audio-ports.h、struct clap_audio_port_infoを参照してください
   int32_t     audio_channel_count;
   const char *audio_port_type;
} clap_track_info_t;

typedef struct clap_plugin_track_info {
   // 情報が変更されたときに呼び出されます。
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_plugin_t *plugin);
} clap_plugin_track_info_t;

typedef struct clap_host_track_info {
   // プラグインが属するトラックに関する情報を取得します。
   // 成功時にtrueを返し、結果をinfoに格納します。
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_host_t *host, clap_track_info_t *info);
} clap_host_track_info_t;

#ifdef __cplusplus
}
#endif