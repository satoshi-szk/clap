#pragma once

#include "../../plugin.h"
#include "../../stream.h"

static CLAP_CONSTEXPR const char CLAP_EXT_UNDO[] = "clap.undo/4";
static CLAP_CONSTEXPR const char CLAP_EXT_UNDO_CONTEXT[] = "clap.undo_context/4";
static CLAP_CONSTEXPR const char CLAP_EXT_UNDO_DELTA[] = "clap.undo_delta/4";

#ifdef __cplusplus
extern "C" {
#endif

/// @page アンドゥ
///
/// この拡張により、プラグインはアンドゥ履歴をホストとマージできます。
/// これにより、ホストと多くのプラグインで共有される単一のアンドゥ履歴が実現されます。
///
/// host->undo()またはhost->redo()を呼び出すことは、ホストのGUI内でアンドゥ/リドゥを
/// クリックすることと同等です。
///
/// プラグインがこのインターフェースを使用する場合、そのアンドゥとリドゥは完全に
/// ホストに委譲される必要があります；プラグインのUIでアンドゥまたはリドゥをクリックすることは、
/// ホストのUIでアンドゥまたはリドゥをクリックすることと同等です。
///
/// いくつかの変更は長時間実行される変更です。例えば、マウスの相互作用は
/// 複雑なデータの編集を開始し、変更を完了するまでに複数のイベントと長時間を要する場合があります。
/// そのような場合、プラグインはhost->begin_change()を呼び出して長時間実行される変更の
/// 開始を示し、host->change_made()を呼び出して変更を完了します。
///
/// ホストは変更をグループ化する場合があります：
/// [---------------------------------]
/// ^-T0      ^-T1    ^-T2            ^-T3
/// ここで、長時間実行される変更C0がT0で開始されます。
/// T1で瞬時変更C1、T2で別の変更C2。
/// その後T3で長時間実行される変更が完了します。
/// ホストは、すべての変更をC0にマージする単一のアンドゥステップを作成します。
///
/// これにより、別の重要な考慮事項が生まれます：長時間実行される変更を開始して
/// 終了しないことは**非常に悪い**ことです。変更の実行中はアンドゥやリドゥを
/// 呼び出すことが不可能だからです。
///
/// 根拠：複数の設計が検討され、この設計は単一のアンドゥ履歴を持つという利点があります。
/// これによりホストの実装が簡素化され、バグが少なく、より堅牢な設計となり、
/// ホス用に一つ、各プラグインインスタンス用に一つという複数のアンドゥコンテキストではなく、
/// 単一のアンドゥコンテキストであるため、ユーザーにとってより簡単な体験となる可能性があります。
///
/// この拡張は、プラグインがホストアンドゥにフックすることを可能な限り簡単にし、
/// デルタを使用することで可能な場合に効率的にすることを試みます。プラグインインターフェースは
/// すべてオプションで、最小限の実装では、プラグインはホストインターフェースのみを使用し、
/// デルタを提供せずにhost->change_made()を呼び出すことができます。これにより、
/// ホストはアンドゥステップのためのプラグイン状態をキャプチャできることを知るのに十分です。

typedef struct clap_undo_delta_properties {
   // trueの場合、プラグインはhost->change_made()でデルタを提供します。
   // falseの場合、clap_undo_delta_propertiesのすべての属性は無関係になります。
   bool has_delta;

   // trueの場合、デルタはディスクに保存でき、プラグインが
   // 指定されたformat_versionと互換性がある限り、将来再利用できます。
   //
   // falseの場合、format_versionはCLAP_INVALID_IDに設定する必要があります。
   bool are_deltas_persistent;

   // これは、プラグインが現在使用しているデルタフォーマットバージョンを表します。
   // 無効な値の場合はCLAP_INVALID_IDを使用してください。
   clap_id format_version;
} clap_undo_delta_properties_t;

// CLAP_EXT_UNDO_DELTAを使用してください。
// これはオプションのインターフェースで、デルタの使用は状態スナップショットの作成に対する最適化です。
typedef struct clap_plugin_undo_delta {
   // プラグインにデルタプロパティを尋ねます。
   // [main-thread]
   void(CLAP_ABI *get_delta_properties)(const clap_plugin_t          *plugin,
                                        clap_undo_delta_properties_t *properties);

   // 指定されたフォーマットバージョンを使用してデルタを適用できるかプラグインに尋ねます。
   // 可能な場合はtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *can_use_delta_format_version)(const clap_plugin_t *plugin,
                                                clap_id              format_version);

   // デルタを使用してアンドゥします。
   // 成功時にtrueを返します。
   //
   // [main-thread]
   bool(CLAP_ABI *undo)(const clap_plugin_t *plugin,
                        clap_id              format_version,
                        const void          *delta,
                        size_t               delta_size);

   // デルタを使用してリドゥします。
   // 成功時にtrueを返します。
   //
   // [main-thread]
   bool(CLAP_ABI *redo)(const clap_plugin_t *plugin,
                        clap_id              format_version,
                        const void          *delta,
                        size_t               delta_size);
} clap_plugin_undo_delta_t;

// CLAP_EXT_UNDO_CONTEXTを使用してください。
// これはオプションのインターフェースで、プラグインが現在の
// アンドゥコンテキストについて知るために実装できます。
typedef struct clap_plugin_undo_context {
   // 現在アンドゥまたはリドゥ操作を実行することが可能かどうかを示します。
   // [main-thread & plugin-subscribed-to-undo-context]
   void(CLAP_ABI *set_can_undo)(const clap_plugin_t *plugin, bool can_undo);
   void(CLAP_ABI *set_can_redo)(const clap_plugin_t *plugin, bool can_redo);

   // 次のアンドゥまたはリドゥステップの名前を設定します。
   // name: ヌル終端文字列。
   // [main-thread & plugin-subscribed-to-undo-context]
   void(CLAP_ABI *set_undo_name)(const clap_plugin_t *plugin, const char *name);
   void(CLAP_ABI *set_redo_name)(const clap_plugin_t *plugin, const char *name);
} clap_plugin_undo_context_t;

// CLAP_EXT_UNDOを使用してください。
typedef struct clap_host_undo {
   // 長時間実行される変更を開始します。
   // プラグインはこれを2回呼び出してはいけません：begin_change()を再び呼び出す前に、
   // cancel_change()またはchange_made()のいずれかを呼び出す必要があります。
   // [main-thread]
   void(CLAP_ABI *begin_change)(const clap_host_t *host);

   // 長時間実行される変更をキャンセルします。
   // cancel_change()は、先行するbegin_change()なしに呼び出してはいけません。
   // [main-thread]
   void(CLAP_ABI *cancel_change)(const clap_host_t *host);

   // アンドゥ可能な変更を完了します。
   // この関数呼び出しの時点で、plugin_state->save()は現在の変更を含むでしょう。
   //
   // name: 変更を説明する必須のヌル終端文字列。これはユーザーに表示されます
   //
   // delta: オプション。アンドゥとリドゥを実行するために使用されるバイナリブロブです。利用できない場合、
   // ホストはプラグイン状態を保存し、state->load()を使用してアンドゥとリドゥを実行します。
   // プラグインはデルタを使用してリドゥ操作を実行できる必要がありますが、アンドゥ操作は
   // delta_can_undoがtrueの場合にのみ可能です。
   //
   // 注記: 提供されたデルタは、増分状態保存とクラッシュ回復に使用される場合があります。
   // プラグインは、フォーマットバージョンIDとバイナリブロブの有効性期間を示すことができます。
   // ホストは、デルタを適用する前にこれらを使用して互換性を確認できます。
   // プラグインがデルタを使用できない場合、ユーザーに通知を提供し、
   // クラッシュ回復は最善努力のジョブを実行し、少なくとも最新の保存された
   // 状態を復元する必要があります。
   //
   // 特別なケース: 共有および同期された状態を持つオブジェクトの場合、変更は報告されるべきではありません。
   // ホストが既にそれについて知っているからです。
   // 例えば、プラグインパラメータの変更はchange_made()の呼び出しを生成すべきではありません。
   //
   // 注記: プラグインがこのインターフェースを要求した場合、host_state->mark_dirty()は
   // 暗黙のアンドゥステップを作成しません。
   //
   // 注記: プラグインがプリセットをロードしたか、大きなデルタにつながる何かを行った場合、
   // デルタを生成しない（nullを渡す）ことを検討し、代わりにホストに状態スナップショットを
   // 作成させる場合があります。
   //
   // 注記: プラグインが短時間内に多くの変更を生成している場合、ホストは
   // それらを単一のアンドゥステップにマージする場合があります。
   //
   // [main-thread]
   void(CLAP_ABI *change_made)(const clap_host_t *host,
                               const char        *name,
                               const void        *delta,
                               size_t             delta_size,
                               bool               delta_can_undo);

   // ホストに次のアンドゥまたはリドゥステップの実行を要求します。
   //
   // 注記: これは複雑で非同期の操作である可能性があり、この関数が
   // 戻った後に完了する場合があります。
   //
   // 注記: 実行するアンドゥ/リドゥステップがない場合、またはホストが
   // その時点でアンドゥ/リドゥを実行できない場合（例：長時間実行される
   // 変更が進行中）、ホストはこの要求を無視する場合があります。
   //
   // [main-thread]
   void(CLAP_ABI *request_undo)(const clap_host_t *host);
   void(CLAP_ABI *request_redo)(const clap_host_t *host);

   // アンドゥコンテキスト情報に登録または登録解除します。
   //
   // このメソッドは、アンドゥコンテキスト情報を更新する際にホストが実行する必要がある
   // 呼び出し数を削減するのに役立ちます。1000+のプラグインを持つ大きなプロジェクトを考えてみてください。
   // プラグインがGUIが表示されアンドゥ/リドゥ情報を表示したい場合にのみコンテキスト情報が
   // 必要である場合に、1000+回の更新を呼び出したくありません。
   //
   // 初期状態は未登録です。
   //
   // is_subscribed: コンテキスト情報を受信するにはtrueに設定
   //
   // このメソッドを使用する場合、プラグインはCLAP_EXT_UNDO_CONTEXTを実装することが必須です。
   //
   // [main-thread]
   void(CLAP_ABI *set_wants_context_updates)(const clap_host_t *host, bool is_subscribed);
} clap_host_undo_t;

#ifdef __cplusplus
}
#endif