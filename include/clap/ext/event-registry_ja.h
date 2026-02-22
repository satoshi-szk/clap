#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_EVENT_REGISTRY[] = "clap.event-registry";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_host_event_registry {
   // イベント空間IDを照会します。
   // 空間ID 0はCLAPのコアイベント用に予約されています。CLAP_CORE_EVENT_SPACEを参照してください。
   //
   // 空間名がホストに不明な場合、falseを返し、*space_idをUINT16_MAXに設定します。
   // [main-thread]
   bool(CLAP_ABI *query)(const clap_host_t *host, const char *space_name, uint16_t *space_id);
} clap_host_event_registry_t;

#ifdef __cplusplus
}
#endif