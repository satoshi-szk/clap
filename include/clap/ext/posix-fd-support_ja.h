#pragma once

#include "../plugin.h"

// この拡張は、プラグインがホストのselect/poll/epoll/kqueueリアクターに
// 自分自身をフックできるようにします。
// これは、メインスレッドで非同期I/Oを処理するのに有用です。
static CLAP_CONSTEXPR const char CLAP_EXT_POSIX_FD_SUPPORT[] = "clap.posix-fd-support";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // IOイベントフラグ、これらは以下を記述するマスクを形成するために使用できます:
   // - 興味があるイベント（register_fd/modify_fd）
   // - 発生したイベント（on_fd）
   CLAP_POSIX_FD_READ = 1 << 0,
   CLAP_POSIX_FD_WRITE = 1 << 1,
   CLAP_POSIX_FD_ERROR = 1 << 2,
};
typedef uint32_t clap_posix_fd_flags_t;

typedef struct clap_plugin_posix_fd_support {
   // このコールバックは「レベルトリガー」です。
   // これは、書き込み可能なfdが継続的に"on_fd()"イベントを生成することを意味します;
   // 書き込みが完了したら、modify_fd()を使用して書き込み通知を削除することを忘れないでください。
   //
   // [main-thread]
   void(CLAP_ABI *on_fd)(const clap_plugin_t *plugin, int fd, clap_posix_fd_flags_t flags);
} clap_plugin_posix_fd_support_t;

typedef struct clap_host_posix_fd_support {
   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *register_fd)(const clap_host_t *host, int fd, clap_posix_fd_flags_t flags);

   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *modify_fd)(const clap_host_t *host, int fd, clap_posix_fd_flags_t flags);

   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *unregister_fd)(const clap_host_t *host, int fd);
} clap_host_posix_fd_support_t;

#ifdef __cplusplus
}
#endif