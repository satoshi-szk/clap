#pragma once

#include "version.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_host {
   clap_version_t clap_version; // CLAP_VERSIONで初期化

   void *host_data; // ホスト用の予約ポインター

   // nameとversionは必須です。
   const char *name;    // 例: "Bitwig Studio"
   const char *vendor;  // 例: "Bitwig GmbH"
   const char *url;     // 例: "https://bitwig.com"
   const char *version; // 例: "4.3"、バージョンフォーマットのアドバイスはplugin.hを参照

   // エクステンションを照会します。
   // 返されたポインターはホストが所有します。
   // plugin->init()の前に呼び出すことは禁止されています。
   // plugin->init()呼び出し内およびその後で呼び出すことができます。
   // [thread-safe]
   const void *(CLAP_ABI *get_extension)(const struct clap_host *host, const char *extension_id);

   // ホストにプラグインを非アクティブ化してから再アクティブ化するよう要求します。
   // 操作はホストによって遅延される場合があります。
   // [thread-safe]
   void(CLAP_ABI *request_restart)(const struct clap_host *host);

   // ホストにプラグインをアクティブ化して処理を開始するよう要求します。
   // これは、外部IOがあり、プラグインを「スリープ」から起こす必要がある場合に有用です。
   // [thread-safe]
   void(CLAP_ABI *request_process)(const struct clap_host *host);

   // ホストにメインスレッドでplugin->on_main_thread(plugin)の呼び出しを
   // スケジュールするよう要求します。
   // このコールバックは可能な限り速やかに呼び出されるべきであり、
   // 通常はホストアプリケーションの次の利用可能なメインスレッドタイムスライスで行われます。
   // 典型的にはコールバックは33ms / 30hz以内に発生します。
   // このガイダンスにもかかわらず、プラグインはメインスレッドコールバックの
   // タイミングの正確性について仮定を立てるべきではありませんが、
   // ホストは迅速であるよう努めるべきです。例えば、高負荷状況において、
   // 環境はオーディオ処理を優先してgui/メインスレッドを枯渇させる可能性があり、
   // ここで示された目安時間よりも大幅に長いコールバック遅延を引き起こす可能性があります。
   // [thread-safe]
   void(CLAP_ABI *request_callback)(const struct clap_host *host);
} clap_host_t;

#ifdef __cplusplus
}
#endif