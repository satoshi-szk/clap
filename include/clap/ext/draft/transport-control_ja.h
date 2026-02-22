#pragma once

#include "../../plugin.h"

// この拡張により、プラグインはホストにトランスポートリクエストを送信できます。
// ホストはこれらのリクエストを実行する義務はないため、インターフェースは
// 部分的に動作する可能性があります。

static CLAP_CONSTEXPR const char CLAP_EXT_TRANSPORT_CONTROL[] = "clap.transport-control/1";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_host_transport_control {
   // 開始ポイントに戻り、トランスポートを開始します
   // [main-thread]
   void(CLAP_ABI *request_start)(const clap_host_t *host);

   // トランスポートを停止し、開始ポイントに移動します
   // [main-thread]
   void(CLAP_ABI *request_stop)(const clap_host_t *host);

   // 再生していない場合、現在の位置からトランスポートを開始します
   // [main-thread]
   void(CLAP_ABI *request_continue)(const clap_host_t *host);

   // 再生中の場合、現在の位置でトランスポートを停止します
   // [main-thread]
   void(CLAP_ABI *request_pause)(const clap_host_t *host);

   // ほとんどのDAWで「スペースバー」が行うのと同等です
   // [main-thread]
   void(CLAP_ABI *request_toggle_play)(const clap_host_t *host);

   // トランスポートを指定された位置にジャンプします。
   // トランスポートは開始されません。
   // [main-thread]
   void(CLAP_ABI *request_jump)(const clap_host_t *host, clap_beattime position);

   // ループ領域を設定します
   // [main-thread]
   void(CLAP_ABI *request_loop_region)(const clap_host_t *host,
                                       clap_beattime      start,
                                       clap_beattime      duration);

   // ループを切り替えます
   // [main-thread]
   void(CLAP_ABI *request_toggle_loop)(const clap_host_t *host);

   // ループを有効/無効にします
   // [main-thread]
   void(CLAP_ABI *request_enable_loop)(const clap_host_t *host, bool is_enabled);

   // 録音を有効/無効にします
   // [main-thread]
   void(CLAP_ABI *request_record)(const clap_host_t *host, bool is_recording);

   // 録音を切り替えます
   // [main-thread]
   void(CLAP_ABI *request_toggle_record)(const clap_host_t *host);
} clap_host_transport_control_t;

#ifdef __cplusplus
}
#endif