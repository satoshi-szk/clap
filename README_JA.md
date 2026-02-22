<p align=center>
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/free-audio/clap/main/artwork/clap-full-logo-white.png">
    <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/free-audio/clap/main/artwork/clap-full-logo-black.png">
    <img alt="CLAP" title="Clever Audio Plugin" src="https://raw.githubusercontent.com/free-audio/clap/main/artwork/clap-full-logo-black.png" width=200>
  </picture>
</p>

- [CLAPについて学ぶ](#clapについて学ぶ)
  - [エントリーポイント](#エントリーポイント)
  - [エクステンション](#エクステンション)
  - [基本的なエクステンション](#基本的なエクステンション)
  - [サポートエクステンション](#サポートエクステンション)
  - [より深いホスト統合](#より深いホスト統合)
  - [サードパーティエクステンション](#サードパーティエクステンション)
- [アダプター](#アダプター)
- [リソース](#リソース)
  - [サンプル](#サンプル)
  - [コミュニティ関連プロジェクト](#コミュニティ関連プロジェクト)
  - [プログラミング言語バインディング](#プログラミング言語バインディング)
  - [アートワーク](#アートワーク)

# CLAPについて学ぶ

CLAPは**CL**ever **A**udio **P**luginの略です。これは、*デジタル・オーディオ・ワークステーション*とオーディオプラグイン（シンセサイザー、オーディオエフェクトなど）が連携するための標準を定義する、安定したABIを提供するインターフェースです。

ABI（**A**pplication **B**inary **I**nterface）は、ホストとプラグインの間のコミュニケーション手段として機能します。これにより後方互換性が提供され、つまりCLAP 1.xでコンパイルされたプラグインバイナリは、任意のCLAP 1.yでロードすることができます。

CLAPを使用するには、[clap/clap_ja.h](include/clap/clap_ja.h)をインクルードしてください。
ドラフトエクステンションも含める場合は、[clap/all_ja.h](include/clap/all_ja.h)をインクルードしてください。

最も重要な2つのオブジェクトは`clap_host`と`clap_plugin`です。

[src/plugin-template.c](src/plugin-template.c)は、CLAPプラグインの配線方法を示す非常にミニマルな例です。

## エントリーポイント

エントリーポイントは[entry_ja.h](include/clap/entry_ja.h)で宣言されています。

## エクステンション

ほとんどの機能はエクステンションから提供されます。これらは実際にはCインターフェースです。
```C
// ホストエクステンション
const clap_host_log *log = host->extension(host, CLAP_EXT_LOG);
if (log)
   log->log(host, CLAP_LOG_INFO, "Hello World! ;^)");

// プラグインエクステンション
const clap_plugin_params *params = plugin->extension(plugin, CLAP_EXT_PARAMS);
if (params)
{
   uint32_t paramsCount = params->count(plugin);
   // ...
}
```

エクステンションは[ext](include/clap/ext)フォルダで定義されています。

一部のエクステンションは設計中であり、[draft](include/clap/ext/draft)フォルダにあります。

エクステンションには以下が含まれます：
- ヘッダー `#include <clap/ext/xxx.h>`
- エクステンション識別子：`#define CLAP_EXT_XXX "clap/XXX"`
- ホストインターフェースは次のように命名：`struct clap_host_xxx`
- プラグインインターフェースは次のように命名：`struct clap_plugin_xxx`
- 各メソッドには明確なスレッド仕様が必要

独自のエクステンションを作成して共有することができます。エクステンション識別子が以下を満たすことを確認してください：
- ABIが壊れる場合のバージョニングを含む
- 一意の識別子である

**すべての文字列は有効なUTF-8です**。

## 基本的なエクステンション

これは、基本的なプラグイン体験を得るために実装し使用したいエクステンションのリストです：
- [state](include/clap/ext/state_ja.h)、プラグインの状態を保存・読み込み
  - [state-context](include/clap/ext/state-context_ja.h)、stateと同じですが、追加のコンテキスト情報付き（プリセット、複製、プロジェクト）
  - [resource-directory](include/clap/ext/draft/resource-directory_ja.h)、プラグインがマルチサンプルなどの追加リソースを保存するためにホストが提供するフォルダ（ドラフト）
- [params](include/clap/ext/params_ja.h)、パラメーター管理
- [note-ports](include/clap/ext/note-ports_ja.h)、ノートポートを定義
- [audio-ports](include/clap/ext/audio-ports_ja.h)、オーディオポートを定義
  - [surround](include/clap/ext/surround_ja.h)、サラウンドチャンネルマッピングを検査
  - [ambisonic](include/clap/ext/draft/ambisonic_ja.h)、アンビソニックチャンネルマッピングを検査
  - [configurable-audio-ports](include/clap/ext/configurable-audio-ports_ja.h)、プラグインに指定された設定を適用するよう要求
  - [audio-ports-config](include/clap/ext/audio-ports-config_ja.h)、事前定義されたオーディオポート設定の簡単なリスト、ユーザーに公開されることを想定
  - [audio-ports-activation](include/clap/ext/audio-ports-activation_ja.h)、指定されたオーディオポートをアクティブ化・非アクティブ化
  - [extensible-audio-ports](include/clap/ext/draft/extensible-audio-ports_ja.h)、ホストがプラグインにオーディオポートを追加できるようにする、動的なオーディオ入力数に有用（ドラフト）
- [render](include/clap/ext/render_ja.h)、リアルタイムまたはオフラインレンダリング
- [latency](include/clap/ext/latency_ja.h)、プラグインのレイテンシーを報告
- [tail](include/clap/ext/tail_ja.h)、処理テール長
- [gui](include/clap/ext/gui_ja.h)、汎用GUIコントローラー
- [voice-info](include/clap/ext/voice-info_ja.h)、プラグインが持つボイス数をホストに知らせる、ポリフォニック変調に重要
- [track-info](include/clap/ext/track-info_ja.h)、プラグインが属するトラックに関する情報を提供
- [tuning](include/clap/ext/draft/tuning_ja.h)、ホストが提供するマイクロチューニング（ドラフト）
- [triggers](include/clap/ext/draft/triggers_ja.h)、プラグインのトリガー、パラメーターに似ているがステートレス

## サポートエクステンション

- [thread-check](include/clap/ext/thread-check_ja.h)、現在どのスレッドにいるかをチェック、正確性検証に有用
- [thread-pool](include/clap/ext/thread-pool_ja.h)、ホストのスレッドプールを使用
- [log](include/clap/ext/log_ja.h)、ホストがプラグインのログを集約
- [timer-support](include/clap/ext/timer-support_ja.h)、プラグインがタイマーハンドラーを登録
- [posix-fd-support](include/clap/ext/posix-fd-support_ja.h)、プラグインがI/Oハンドラーを登録

## より深いホスト統合

- [remote-controls](include/clap/ext/remote-controls_ja.h)、8つのノブを持つコントローラーにマップできるコントロールバンク
- [preset-discovery](include/clap/factory/preset-discovery_ja.h)、ホストがプラグインのプリセットをネイティブファイル形式でインデックス化
- [preset-load](include/clap/ext/preset-load_ja.h)、ホストがプラグインにプリセットの読み込みを要求
- [param-indication](include/clap/ext/param-indication_ja.h)、物理コントロールがパラメーターにマップされ、オートメーションデータがある場合にプラグインに知らせる
- [note-name](include/clap/ext/note-name_ja.h)、ノートに名前を付ける、ドラムマシンに有用
- [transport-control](include/clap/ext/draft/transport-control_ja.h)、プラグインがホストのトランスポートを制御（ドラフト）
- [context-menu](include/clap/ext/context-menu_ja.h)、ホストとプラグイン間でコンテキストメニューエントリを交換、プラグインがホストに独自のコンテキストメニューのポップアップを要求

## サードパーティエクステンション

- [`cockos.reaper_extension`](https://github.com/justinfrankel/reaper-sdk/blob/main/reaper-plugins/reaper_plugin.h#L138)、[REAPER](http://reaper.fm) APIへのアクセス

# アダプター

- [clap-wrapper](https://github.com/free-audio/clap-wrapper)、他のプラグイン環境でCLAPを使用するためのラッパー

# リソース

- [clap-validator](https://github.com/robbert-vdh/clap-validator)、CLAPプラグインのバリデーターと自動テストスイート
- [clapdb](https://clapdb.tech)、CLAPをサポートするプラグインとDAWのリスト

## サンプル

- [clap-host](https://github.com/free-audio/clap-host)、非常にシンプルなホスト
- [clap-plugins](https://github.com/free-audio/clap-plugins)、非常にシンプルなプラグイン

## コミュニティ関連プロジェクト

- [clap-juce-extension](https://github.com/free-audio/clap-juce-extension)、juceアドオン
- [MIP2](https://github.com/skei/MIP2)、ホストとプラグイン
- [Avendish](https://github.com/celtera/avendish)、Clapをサポートする、C++でのメディアプラグイン用のリフレクションベースAPI
- [NIH-plug](https://github.com/robbert-vdh/nih-plug)、ボイラープレートを削減しながら邪魔にならないことを目指すAPI不可知論的なRustベースのプラグインフレームワーク
- [iPlug2](https://iplug2.github.io)、Clapをサポートする、リベラルライセンスのC++オーディオプラグインフレームワーク

## プログラミング言語バインディング

- [clap-sys](https://github.com/glowcoil/clap-sys)、Rustバインディング
- [CLAP-for-Delphi](https://github.com/Bremmers/CLAP-for-Delphi)、Delphiバインディング
- [clap-zig-bindings](https://sr.ht/~interpunct/clap-zig-bindings/)、Zigバインディング
- [CLAP for Ada](https://github.com/ficorax/cfa)、Ada 2012バインディング

## アートワーク

 - [CLAP Logo Pack.zip](https://github.com/free-audio/clap/files/8805281/CLAP.Logo.Pack.zip)