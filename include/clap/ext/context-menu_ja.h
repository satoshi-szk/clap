#pragma once

#include "../plugin.h"

// この拡張は、ホストとプラグインがメニューアイテムを交換し、
// プラグインがホストにコンテキストメニューの表示を要求することを可能にします。

static CLAP_CONSTEXPR const char CLAP_EXT_CONTEXT_MENU[] = "clap.context-menu/1";

// 最新ドラフトは100%互換です。
// この互換IDは2026年に削除される可能性があります。
static CLAP_CONSTEXPR const char CLAP_EXT_CONTEXT_MENU_COMPAT[] = "clap.context-menu.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

// コンテキストメニューには異なるターゲット種類があります
enum {
   CLAP_CONTEXT_MENU_TARGET_KIND_GLOBAL = 0,
   CLAP_CONTEXT_MENU_TARGET_KIND_PARAM = 1,
};

// コンテキストメニューターゲットを記述します
typedef struct clap_context_menu_target {
   uint32_t kind;
   clap_id  id;
} clap_context_menu_target_t;

enum {
   // クリック可能なメニューエントリを追加します。
   // data: const clap_context_menu_item_entry_t*
   CLAP_CONTEXT_MENU_ITEM_ENTRY,

   // チェックマークとラベルの両方を特徴とするクリック可能なメニューエントリを追加します。
   // data: const clap_context_menu_item_check_entry_t*
   CLAP_CONTEXT_MENU_ITEM_CHECK_ENTRY,

   // セパレーター線を追加します。
   // data: NULL
   CLAP_CONTEXT_MENU_ITEM_SEPARATOR,

   // 指定されたラベルでサブメニューを開始します。
   // data: const clap_context_menu_item_begin_submenu_t*
   CLAP_CONTEXT_MENU_ITEM_BEGIN_SUBMENU,

   // 現在のサブメニューを終了します。
   // data: NULL
   CLAP_CONTEXT_MENU_ITEM_END_SUBMENU,

   // タイトルエントリを追加します
   // data: const clap_context_menu_item_title_t *
   CLAP_CONTEXT_MENU_ITEM_TITLE,
};
typedef uint32_t clap_context_menu_item_kind_t;

typedef struct clap_context_menu_entry {
   // 表示されるテキスト
   const char *label;

   // falseの場合、メニューエントリはグレーアウトされ、クリックできません
   bool        is_enabled;
   clap_id     action_id;
} clap_context_menu_entry_t;

typedef struct clap_context_menu_check_entry {
   // 表示されるテキスト
   const char *label;

   // falseの場合、メニューエントリはグレーアウトされ、クリックできません
   bool        is_enabled;

   // trueの場合、メニューエントリはチェック済みとして表示されます
   bool        is_checked;
   clap_id     action_id;
} clap_context_menu_check_entry_t;

typedef struct clap_context_menu_item_title {
   // 表示されるテキスト
   const char *title;

   // falseの場合、メニューエントリはグレーアウトされます
   bool        is_enabled;
} clap_context_menu_item_title_t;

typedef struct clap_context_menu_submenu {
   // 表示されるテキスト
   const char *label;

   // falseの場合、メニューエントリはグレーアウトされ、サブメニューを表示しません
   bool        is_enabled;
} clap_context_menu_submenu_t;

// コンテキストメニュービルダー。
// このオブジェクトはスレッドセーフではなく、提供されたのと同じスレッドで使用される必要があります。
typedef struct clap_context_menu_builder {
   void *ctx;

   // メニューにエントリを追加します。
   // item_dataの型はitem_kindによって決定されます。
   // 成功時にtrueを返します。
   bool(CLAP_ABI *add_item)(const struct clap_context_menu_builder *builder,
                            clap_context_menu_item_kind_t           item_kind,
                            const void                             *item_data);

   // メニュービルダーが指定されたアイテム種類をサポートしている場合trueを返します
   bool(CLAP_ABI *supports)(const struct clap_context_menu_builder *builder,
                            clap_context_menu_item_kind_t           item_kind);
} clap_context_menu_builder_t;

typedef struct clap_plugin_context_menu {
   // プラグインのメニューアイテムをメニュービルダーに挿入します。
   // targetがnullの場合、グローバルコンテキストを想定します。
   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *populate)(const clap_plugin_t               *plugin,
                            const clap_context_menu_target_t  *target,
                            const clap_context_menu_builder_t *builder);

   // populate()を介して以前にホストに提供されたアクションを実行します。
   // targetがnullの場合、グローバルコンテキストを想定します。
   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *perform)(const clap_plugin_t              *plugin,
                           const clap_context_menu_target_t *target,
                           clap_id                           action_id);
} clap_plugin_context_menu_t;

typedef struct clap_host_context_menu {
   // ホストのメニューアイテムをメニュービルダーに挿入します。
   // targetがnullの場合、グローバルコンテキストを想定します。
   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *populate)(const clap_host_t                 *host,
                            const clap_context_menu_target_t  *target,
                            const clap_context_menu_builder_t *builder);

   // populate()を介して以前にプラグインに提供されたアクションを実行します。
   // targetがnullの場合、グローバルコンテキストを想定します。
   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *perform)(const clap_host_t                *host,
                           const clap_context_menu_target_t *target,
                           clap_id                           action_id);

   // ホストがプラグイン用のポップアップメニューを表示できる場合trueを返します。
   // これは、プラグインを表示するために使用される現在のウィンドウシステムに
   // 依存する可能性があるため、プラグインウィンドウの作成後に戻り値は無効化されます。
   // [main-thread]
   bool(CLAP_ABI *can_popup)(const clap_host_t *host);

   // 指定されたパラメータのホストポップアップメニューを表示します。
   // プラグインが埋め込みGUIを使用している場合、xとyはプラグインのウィンドウに
   // 相対的です。そうでなければ、それらは絶対座標であり、
   // スクリーンインデックスがそれに応じて設定される可能性があります。
   // targetがnullの場合、グローバルコンテキストを想定します。
   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *popup)(const clap_host_t                *host,
                         const clap_context_menu_target_t *target,
                         int32_t                           screen_index,
                         int32_t                           x,
                         int32_t                           y);
} clap_host_context_menu_t;

#ifdef __cplusplus
}
#endif