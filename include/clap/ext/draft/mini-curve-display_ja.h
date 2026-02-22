#pragma once

#include "../../plugin.h"

// この拡張により、ホストはプラグインによって提供される小さなカーブをレンダリングできます。
// 有用な応用例は、DAWミキサービューでEQの周波数応答をレンダリングすることです。

static CLAP_CONSTEXPR const char CLAP_EXT_MINI_CURVE_DISPLAY[] = "clap.mini-curve-display/3";

#ifdef __cplusplus
extern "C" {
#endif

enum clap_mini_curve_display_curve_kind {
   // カーブの種類が提案された種類のいずれにも当てはまらない場合、これを使用し、
   // おそらくリストを拡張するためのプルリクエストを作成してください。
   CLAP_MINI_CURVE_DISPLAY_CURVE_KIND_UNSPECIFIED = 0,

   // ミニカーブは、プラグインの総ゲイン応答を描画することを目的としています。
   // この場合、y値はdB、x値はHz（対数）です。
   // これは例えばイコライザーで有用です。
   CLAP_MINI_CURVE_DISPLAY_CURVE_KIND_GAIN_RESPONSE = 1,

   // ミニカーブは、プラグインの総位相応答を描画することを目的としています。
   // この場合、y値はラジアン、x値はHz（対数）です。
   // これは例えばイコライザーで有用です。
   CLAP_MINI_CURVE_DISPLAY_CURVE_KIND_PHASE_RESPONSE = 2,

   // ミニカーブは、プラグインの伝達カーブを描画することを目的としています。
   // この場合、xとyの両方の値はdBです。
   // これは例えばコンプレッサーやディストーションプラグインで有用です。
   CLAP_MINI_CURVE_DISPLAY_CURVE_KIND_TRANSFER_CURVE = 3,

   // このミニカーブは、時間に対するゲインリダクションを描画することを目的としています。
   // この場合、xは秒単位のウィンドウ、yはdB単位のレベルを表し、x_minは
   // 常に0、x_maxはウィンドウの持続時間です。
   // これは例えばコンプレッサーやリミッターで有用です。
   CLAP_MINI_CURVE_DISPLAY_CURVE_KIND_GAIN_REDUCTION = 4,

   // このカーブは一般的な時系列プロットを意図しています。この場合
   // xは秒単位のウィンドウを表します。x_minは常に0、x_maxはウィンドウの持続時間です。
   // Yは指定されておらず、プラグイン次第です。
   CLAP_MINI_CURVE_DISPLAY_CURVE_KIND_TIME_SERIES = 5,

   // 注：将来的にここにさらなるエントリが追加される可能性があります
};

typedef struct clap_mini_curve_display_curve_hints {

   // X軸の範囲。
   double x_min;
   double x_max;

   // Y軸の範囲。
   double y_min;
   double y_max;

} clap_mini_curve_display_curve_hints_t;

// 描画されるカーブを表現する点のセット。
typedef struct clap_mini_curve_display_curve_data {
   // これらの値が表すカーブの種類を示し、ホストはこの情報を使用して
   // 意味のある色でカーブを描画できます。
   int32_t curve_kind;

   // values[0]は最左の値、values[data_size -1]は最右の値になります。
   //
   // 値0とUINT16_MAXは描画されません。
   // 値1はカーブの下部、UINT16_MAX - 1は上部になります。
   uint16_t *values;
   uint32_t  values_count;
} clap_mini_curve_display_curve_data_t;

typedef struct clap_plugin_mini_curve_display {
   // プラグインが描画したいカーブの数を返します。
   // これらのカーブを表示するスペースは小さく、データが多すぎると
   // 出力が読みにくくなることに注意してください。
   uint32_t(CLAP_ABI *get_curve_count)(const clap_plugin_t *plugin);

   // 各カーブバッファにカーブをレンダリングします。
   //
   // curvesは配列で、各エントリ（curves_sizeまで）には
   // プラグインによって埋められる必要のある事前割り当てされた
   // valuesバッファが含まれています。
   //
   // ホストは最初のカーブから最後のカーブまで、カーブを「スタック」します。
   // curves[0]は最初に描画されるカーブです。
   // curves[n + 1]はcurves[n]の上に描画されます。
   //
   // レンダリングされたカーブの数を返します。
   // [main-thread]
   uint32_t(CLAP_ABI *render)(const clap_plugin_t                  *plugin,
                              clap_mini_curve_display_curve_data_t *curves,
                              uint32_t                              curves_size);

   // カーブが現在観察されているかどうかをプラグインに伝えます。
   // 観察されていない場合、render()は呼び出せません。
   //
   // is_observedがtrueになると、カーブの内容と軸名は暗黙的に無効化されます。
   // そのため、プラグインはhost->changedを呼び出す必要がありません。
   //
   // [main-thread]
   void(CLAP_ABI *set_observed)(const clap_plugin_t *plugin, bool is_observed);

   // 軸名を取得します。
   // x_nameとy_nameはnullであってはいけません。
   // 成功時、名前容量が十分だった場合はtrueを返します。
   // [main-thread]
   bool(CLAP_ABI *get_axis_name)(const clap_plugin_t *plugin,
                                 uint32_t             curve_index,
                                 char                *x_name,
                                 char                *y_name,
                                 uint32_t             name_capacity);
} clap_plugin_mini_curve_display_t;

enum clap_mini_curve_display_change_flags {
   // カーブの内容が変更されたことをホストに通知します。
   // カーブが観察されており、静的な場合のみ呼び出せます。
   CLAP_MINI_CURVE_DISPLAY_CURVE_CHANGED = 1 << 0,

   // カーブの軸名が変更されたことをホストに通知します。
   // カーブが観察されている場合のみ呼び出せます。
   CLAP_MINI_CURVE_DISPLAY_AXIS_NAME_CHANGED = 1 << 1,
};

typedef struct clap_host_mini_curve_display {
   // 指定されたclap_mini_display_curve_hints_t構造体を埋めて、
   // 成功した場合はtrueを返します。そうでない場合はfalseを返します。
   // [main-thread]
   bool(CLAP_ABI *get_hints)(const clap_host_t                     *host,
                             uint32_t                               kind,
                             clap_mini_curve_display_curve_hints_t *hints);

   // カーブを静的または動的としてマークします。
   // カーブは最初は静的と見なされますが、プラグインは明示的に
   // この状態を初期化する必要があります。
   //
   // 静的な場合、カーブの変更はhost->changed()を呼び出すことで通知されます。
   // 動的な場合、カーブは常に変化しており、ホストは定期的に
   // 再レンダリングすることが期待されます。
   //
   // [main-thread]
   void(CLAP_ABI *set_dynamic)(const clap_host_t *host, bool is_dynamic);

   // clap_mini_curve_display_change_flagsを参照
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host, uint32_t flags);
} clap_host_mini_curve_display_t;

#ifdef __cplusplus
}
#endif