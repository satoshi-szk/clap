#pragma once

#include "../plugin.h"

/// @page GUI
///
/// この拡張は、プラグインがGUIを表示する方法を定義します。
///
/// 2つのアプローチがあります:
/// 1. プラグインがウィンドウを作成し、ホストのウィンドウに埋め込む
/// 2. プラグインがフローティングウィンドウを作成する
///
/// ウィンドウの埋め込みはホストにより多くの制御を与え、より統合された感じを与えます。
/// フローティングウィンドウは、技術的な制限により唯一の選択肢である場合があります。
///
/// 埋め込みプロトコルは圧倒的に最も一般的で、これまでのすべてのホストでサポートされており、
/// プラグイン作成者は少なくともそのケースをサポートすべきです。
///
/// GUIの表示は以下のように動作します:
///  1. clap_plugin_gui->is_api_supported()、何が動作するかチェック
///  2. clap_plugin_gui->create()、guiリソースを割り当て
///  3. プラグインウィンドウがフローティングの場合
///  4.    -> clap_plugin_gui->set_transient()
///  5.    -> clap_plugin_gui->suggest_title()
///  6. そうでなければ
///  7.    -> clap_plugin_gui->set_scale()
///  8.    -> clap_plugin_gui->can_resize()
///  9.    -> リサイズ可能で前のセッションから既知のサイズがある場合、clap_plugin_gui->set_size()
/// 10.    -> そうでなければ clap_plugin_gui->get_size()、初期サイズを取得
/// 11.    -> clap_plugin_gui->set_parent()
/// 12. clap_plugin_gui->show()
/// 13. clap_plugin_gui->hide()/show() ...
/// 14. guiが完了したら clap_plugin_gui->destroy()
///
/// ウィンドウのリサイズ（埋め込みの場合、プラグインによって開始）:
/// 1. プラグインが clap_host_gui->request_resize() を呼び出す
/// 2. ホストがtrueを返した場合、新しいサイズが受け入れられ、
///    ホストは clap_plugin_gui->set_size() を呼び出す必要がありません。
///    ホストがfalseを返した場合、新しいサイズは拒否されます。
///
/// ウィンドウのリサイズ（埋め込みの場合、ドラッグ）:
/// 1. clap_plugin_gui->can_resize() がtrueを返す場合のみ可能
/// 2. マウスドラッグ -> new_size
/// 3. clap_plugin_gui->adjust_size(new_size) -> working_size
/// 4. clap_plugin_gui->set_size(working_size)

static CLAP_CONSTEXPR const char CLAP_EXT_GUI[] = "clap.gui";

// ウィンドウAPIがここにリストされていない場合は、issueを開いて対応します。
// https://github.com/free-audio/clap/issues/new

// 物理サイズを使用
// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setparent を使用して埋め込み
static const CLAP_CONSTEXPR char CLAP_WINDOW_API_WIN32[] = "win32";

// 論理サイズを使用、clap_plugin_gui->set_scale() を呼び出さない
static const CLAP_CONSTEXPR char CLAP_WINDOW_API_COCOA[] = "cocoa";

// 物理サイズを使用
// https://specifications.freedesktop.org/xembed-spec/xembed-spec-latest.html を使用して埋め込み
static const CLAP_CONSTEXPR char CLAP_WINDOW_API_X11[] = "x11";

// 物理サイズを使用
// 埋め込みは現在サポートされていません、フローティングウィンドウを使用
static const CLAP_CONSTEXPR char CLAP_WINDOW_API_WAYLAND[] = "wayland";

#ifdef __cplusplus
extern "C" {
#endif

typedef void         *clap_hwnd;
typedef void         *clap_nsview;
typedef unsigned long clap_xwnd;

// ウィンドウ参照を表します。
typedef struct clap_window {
   const char *api; // CLAP_WINDOW_API_XXX のいずれか
   union {
      clap_nsview cocoa;
      clap_xwnd   x11;
      clap_hwnd   win32;
      void       *ptr; // clap外で定義されたもの用
   };
} clap_window_t;

// ホストまたはウィンドウマネージャーによって開始されるウィンドウリサイズを改善するための情報。
typedef struct clap_gui_resize_hints {
   bool can_resize_horizontally;
   bool can_resize_vertically;

   // 水平と垂直の両方のリサイズが利用可能な場合、アスペクト比を保持するか、
   // そうである場合、保持すべき幅 x 高さのアスペクト比は何か。
   // これらのフラグは、can_resize_horizontally または vertically がfalseの場合使用されず、
   // preserveがfalseの場合、比率は使用されません。
   bool     preserve_aspect_ratio;
   uint32_t aspect_ratio_width;
   uint32_t aspect_ratio_height;
} clap_gui_resize_hints_t;

// サイズ（幅、高さ）はピクセル単位です; 対応するウィンドウシステム拡張が
// 物理ピクセルか論理ピクセルかを定義する責任があります。
typedef struct clap_plugin_gui {
   // 要求されたgui apiがサポートされている場合trueを返します。
   // フローティング（プラグイン作成）または非フローティング（埋め込み）モードのいずれかで。
   // [main-thread]
   bool(CLAP_ABI *is_api_supported)(const clap_plugin_t *plugin, const char *api, bool is_floating);

   // プラグインが推奨apiを持っている場合trueを返します。
   // ホストはプラグインの設定を尊重する義務はありません、これは単なるヒントです。
   // const char **api変数は、上記で定義されたCLAP_WINDOW_API_定数の1つへの
   // ポインターとして明示的に割り当てられるべきであり、strcopyedではありません。
   // [main-thread]
   bool(CLAP_ABI *get_preferred_api)(const clap_plugin_t *plugin,
                                     const char         **api,
                                     bool                *is_floating);

   // guiに必要なすべてのリソースを作成および割り当てます。
   //
   // is_floatingがtrueの場合、ウィンドウはホストによって管理されません。プラグインは
   // ウィンドウを親ウィンドウの上に留まるように設定できます。set_transient()を参照してください。
   // フローティングウィンドウの場合、apiはnullまたは空白でも構いません。
   //
   // is_floatingがfalseの場合、プラグインはウィンドウを親ウィンドウに埋め込む必要があります。
   // set_parent()を参照してください。
   //
   // この呼び出し後、GUIはまだ表示されていない可能性があります; show()の呼び出しを忘れないでください。
   //
   // GUIが正常に作成された場合trueを返します。
   // [main-thread]
   bool(CLAP_ABI *create)(const clap_plugin_t *plugin, const char *api, bool is_floating);

   // guiに関連するすべてのリソースを解放します。
   // [main-thread]
   void(CLAP_ABI *destroy)(const clap_plugin_t *plugin);

   // 絶対GUI拡大率を設定し、OS情報をオーバーライドします。
   // ウィンドウapiが論理ピクセルに依存している場合は使用すべきではありません。
   //
   // プラグインがOSに直接照会して拡大率を自分で算出することを好む場合は、
   // 呼び出しを無視してください。
   //
   // scale = 2 は200%拡大を意味します。
   //
   // 拡大を適用できた場合trueを返します
   // 呼び出しが無視されたか、拡大を適用できなかった場合falseを返します。
   // [main-thread]
   bool(CLAP_ABI *set_scale)(const clap_plugin_t *plugin, double scale);

   // プラグインUIの現在のサイズを取得します。
   // サイズを尋ねる前に clap_plugin_gui->create() が呼び出されている必要があります。
   //
   // プラグインがサイズを取得できた場合trueを返します。
   // [main-thread]
   bool(CLAP_ABI *get_size)(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height);

   // ウィンドウがリサイズ可能（マウスドラッグ）な場合trueを返します。
   // [main-thread & !floating]
   bool(CLAP_ABI *can_resize)(const clap_plugin_t *plugin);

   // プラグインがウィンドウのリサイズ方法についてヒントを提供できる場合trueを返します。
   // [main-thread & !floating]
   bool(CLAP_ABI *get_resize_hints)(const clap_plugin_t *plugin, clap_gui_resize_hints_t *hints);

   // プラグインguiがリサイズ可能な場合、プラグインは指定されたサイズに
   // 収まる最も近い使用可能なサイズを計算します。
   // このメソッドはサイズを変更しません。
   //
   // プラグインが指定されたサイズを調整できた場合trueを返します。
   // [main-thread & !floating]
   bool(CLAP_ABI *adjust_size)(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height);

   // ウィンドウサイズを設定します。
   //
   // プラグインがウィンドウを指定されたサイズにリサイズできた場合trueを返します。
   // [main-thread & !floating]
   bool(CLAP_ABI *set_size)(const clap_plugin_t *plugin, uint32_t width, uint32_t height);

   // プラグインウィンドウを指定されたウィンドウに埋め込みます。
   //
   // 成功時にtrueを返します。
   // [main-thread & !floating]
   bool(CLAP_ABI *set_parent)(const clap_plugin_t *plugin, const clap_window_t *window);

   // プラグインフローティングウィンドウを指定されたウィンドウの上に留まるように設定します。
   //
   // 成功時にtrueを返します。
   // [main-thread & floating]
   bool(CLAP_ABI *set_transient)(const clap_plugin_t *plugin, const clap_window_t *window);

   // ウィンドウタイトルを提案します。フローティングウィンドウのみ。
   //
   // [main-thread & floating]
   void(CLAP_ABI *suggest_title)(const clap_plugin_t *plugin, const char *title);

   // ウィンドウを表示します。
   //
   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *show)(const clap_plugin_t *plugin);

   // ウィンドウを非表示にします。このメソッドはリソースを解放せず、
   // ウィンドウコンテンツを非表示にするだけです。ただし、描画タイマーを停止することは良いアイデアです。
   //
   // 成功時にtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *hide)(const clap_plugin_t *plugin);
} clap_plugin_gui_t;

typedef struct clap_host_gui {
   // ホストは get_resize_hints() を再度呼び出すべきです。
   // [thread-safe & !floating]
   void(CLAP_ABI *resize_hints_changed)(const clap_host_t *host);

   // ホストにクライアントエリアを width、height にリサイズするよう要求します。
   // 新しいサイズが受け入れられた場合true、そうでなければfalseを返します。
   // ホストは set_size() を呼び出す必要がありません。
   //
   // 注意: メインスレッドから呼び出されない場合、戻り値は単にホストが
   // 要求を認識し、非同期で処理することを意味します。要求が満たせない場合、
   // ホストは set_size() を呼び出して操作を元に戻します。
   // [thread-safe & !floating]
   bool(CLAP_ABI *request_resize)(const clap_host_t *host, uint32_t width, uint32_t height);

   // ホストにプラグインguiの表示を要求します。
   // 成功時にtrue、そうでなければfalseを返します。
   // [thread-safe]
   bool(CLAP_ABI *request_show)(const clap_host_t *host);

   // ホストにプラグインguiの非表示を要求します。
   // 成功時にtrue、そうでなければfalseを返します。
   // [thread-safe]
   bool(CLAP_ABI *request_hide)(const clap_host_t *host);

   // フローティングウィンドウが閉じられたか、guiへの接続が失われました。
   //
   // was_destroyedがtrueの場合、ホストは gui破棄を認識するために
   // clap_plugin_gui->destroy() を呼び出す必要があります。
   // [thread-safe]
   void(CLAP_ABI *closed)(const clap_host_t *host, bool was_destroyed);
} clap_host_gui_t;

#ifdef __cplusplus
}
#endif