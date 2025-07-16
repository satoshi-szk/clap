#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_TIMER_SUPPORT[] = "clap.timer-support";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_timer_support {
   // [main-thread]
   void(CLAP_ABI *on_timer)(const clap_plugin_t *plugin, clap_id timer_id);
} clap_plugin_timer_support_t;

typedef struct clap_host_timer_support {
   // 定期タイマーを登録します。
   // ホストは、期間が特定のしきい値を下回る場合、期間を調整する可能性があります。
   // 30 Hzは許可されるべきです。
   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *register_timer)(const clap_host_t *host, uint32_t period_ms, clap_id *timer_id);

   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *unregister_timer)(const clap_host_t *host, clap_id timer_id);
} clap_host_timer_support_t;

#ifdef __cplusplus
}
#endif