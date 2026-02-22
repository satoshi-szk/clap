#pragma once

#include "../../color.h"
#include "../../plugin.h"
#include "../../string-sizes.h"

// この拡張により、ホストはプロジェクトまたはセッション内でのプラグインの位置について
// より詳細な情報をプラグインに伝えることができます。

static CLAP_CONSTEXPR const char CLAP_EXT_PROJECT_LOCATION[] = "clap.project-location/2";

#ifdef __cplusplus
extern "C" {
#endif

enum clap_project_location_kind {
   // ドキュメント/プロジェクト/セッションを表します。
   CLAP_PROJECT_LOCATION_PROJECT = 1,

   // トラックのグループを表します。
   // トラックグループ、トラック、デバイス（ポストプロセッシング）を含むことができます。
   // トラックグループ内の最初のデバイスのインデックスは、
   // このグループ内の最後のトラックまたはトラックグループ + 1です。
   CLAP_PROJECT_LOCATION_TRACK_GROUP = 2,

   // 単一のトラックを表します。
   // デバイス（シリアル）を含みます。
   CLAP_PROJECT_LOCATION_TRACK = 3,

   // 単一のデバイスを表します。
   // 他のネストされたデバイスチェーンを含むことができます。
   CLAP_PROJECT_LOCATION_DEVICE = 4,

   // ネストされたデバイスチェーン（シリアル）を表します。
   // 親はデバイスでなければなりません。
   // 他のデバイスを含みます。
   CLAP_PROJECT_LOCATION_NESTED_DEVICE_CHAIN = 5,
};

enum clap_project_location_track_kind {
   // このトラックはインストゥルメントトラックです。
   CLAP_PROJECT_LOCATION_INSTUMENT_TRACK = 1,

   // このトラックはオーディオトラックです。
   CLAP_PROJECT_LOCATION_AUDIO_TRACK = 2,

   // このトラックはインストゥルメントとオーディオの両方のトラックです。
   CLAP_PROJECT_LOCATION_HYBRID_TRACK = 3,

   // このトラックはリターントラックです。
   CLAP_PROJECT_LOCATION_RETURN_TRACK = 4,

   // このトラックはマスタートラックです。
   // 各グループは、すべての子トラックの合計を処理するためのマスタートラックを持ちます。
   CLAP_PROJECT_LOCATION_MASTER_TRACK = 5,
};

enum clap_project_location_flags {
   CLAP_PROJECT_LOCATION_HAS_INDEX = 1 << 0,
   CLAP_PROJECT_LOCATION_HAS_COLOR = 1 << 1,
};

typedef struct clap_project_location_element {
   // ビットマスク、clap_project_location_flagsを参照してください。
   uint64_t flags;

   // 要素の種類、CLAP_PROJECT_LOCATION_*値のいずれかでなければなりません。
   uint32_t kind;

   // kindがCLAP_PLUGIN_LOCATION_TRACKの場合のみ関連します。
   // enum CLAP_PROJECT_LOCATION_track_kindを参照してください。
   uint32_t track_kind;

   // 親要素内でのインデックス。
   // flagsでCLAP_PROJECT_LOCATION_HAS_INDEXが設定されている場合のみ使用可能です。
   uint32_t index;

   // 要素の内部ID。
   // これはユーザーへの表示を目的としたものではなく、
   // ホストに高速な検索方法を提供するためのものです。
   char id[CLAP_PATH_SIZE];

   // 要素のユーザーフレンドリーな名前。
   char name[CLAP_NAME_SIZE];

   // この要素の色。
   // flagsでCLAP_PROJECT_LOCATION_HAS_COLORが設定されている場合のみ使用可能です。
   clap_color_t color;
} clap_project_location_element_t;

typedef struct clap_plugin_project_location {
   // プラグインインスタンスの位置が変更された時にホストによって呼び出されます。
   //
   // この配列の最後の項目は常にデバイス自体を参照し、
   // CLAP_PLUGIN_LOCATION_DEVICE種類であることが期待されます。
   // この配列の最初の項目は常にこのデバイスが属するプロジェクトを参照し、
   // CLAP_PROJECT_LOCATION_PROJECT種類でなければなりません。パスは次のようなものが期待されます：
   // PROJECT > TRACK_GROUP+ > TRACK > (DEVICE > NESTED_DEVICE_CHAIN)* > DEVICE
   //
   // [main-thread]
   void(CLAP_ABI *set)(const clap_plugin_t                  *plugin,
                       const clap_project_location_element_t *path,
                       uint32_t                              num_elements);
} clap_plugin_project_location_t;

#ifdef __cplusplus
}
#endif