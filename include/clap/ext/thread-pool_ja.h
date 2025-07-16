#pragma once

#include "../plugin.h"

/// @page
///
/// この拡張は、プラグインがホストのスレッドプールを使用することを可能にします。
///
/// プラグインは@ref clap_plugin_thread_poolを提供する必要があり、
/// ホストは@ref clap_host_thread_poolを提供する場合があります。
/// 提供しない場合、プラグインは独自の手段でデータを処理すべきです。
/// 最悪の場合、単一スレッドのfor-loopです。
///
/// 処理するN個のボイスでの簡単な例
///
/// @code
/// void myplug_thread_pool_exec(const clap_plugin *plugin, uint32_t voice_index)
/// {
///    compute_voice(plugin, voice_index);
/// }
///
/// void myplug_process(const clap_plugin *plugin, const clap_process *process)
/// {
///    ...
///    bool didComputeVoices = false;
///    if (host_thread_pool && host_thread_pool.exec)
///       didComputeVoices = host_thread_pool.request_exec(host, plugin, N);
///
///    if (!didComputeVoices)
///       for (uint32_t i = 0; i < N; ++i)
///          myplug_thread_pool_exec(plugin, i);
///    ...
/// }
/// @endcode
///
/// スレッドプールを使用すると、関連するスレッド同期により
/// ハードリアルタイムルールが破られる可能性があることに注意してください。
///
/// ホストがハードリアルタイムプレッシャー下で動作していることを知っている場合、
/// このインターフェースを提供しないことを決定する場合があります。

static CLAP_CONSTEXPR const char CLAP_EXT_THREAD_POOL[] = "clap.thread-pool";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_thread_pool {
   // スレッドプールによって呼び出される
   void(CLAP_ABI *exec)(const clap_plugin_t *plugin, uint32_t task_index);
} clap_plugin_thread_pool_t;

typedef struct clap_host_thread_pool {
   // ホストスレッドプールでnum_tasksジョブをスケジュールします。
   // 同時に、またはスレッドプールから呼び出すことはできません。
   // すべてのタスクが処理されるまでブロックします。
   // これは、プロセス呼び出し内でのリアルタイム処理専用に使用する必要があります。
   // ホストがすべてのタスクを実行した場合はtrue、要求を拒否した場合はfalseを返します。
   // ホストは、プラグインがプロセス呼び出し内にあるかどうかをチェックし、
   // そうでない場合は実行要求を拒否すべきです。
   // [audio-thread]
   bool(CLAP_ABI *request_exec)(const clap_host_t *host, uint32_t num_tasks);
} clap_host_thread_pool_t;

#ifdef __cplusplus
}
#endif