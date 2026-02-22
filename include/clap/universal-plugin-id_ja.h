#pragma once

// プラグインABIとプラグイン識別子のペア。
//
// 他のフォーマットを表現したい場合は、abiの名前とidの表現を含む
// コメントの更新を送ってください。
typedef struct clap_universal_plugin_id {
   // プラグインABI名、小文字でnullで終端。
   // 例: "clap", "vst3", "vst2", "au", ...
   const char *abi;

   // プラグインID、nullで終端され、以下のようにフォーマットされます:
   //
   // CLAP: プラグインidを使用
   //   例: "com.u-he.diva"
   //
   // AU: 文字列を"type:subt:manu"のようにフォーマット
   //   例: "aumu:SgXT:VmbA"
   //
   // VST2: idを符号付き32ビット整数として出力
   //   例: "-4382976"
   //
   // VST3: idを標準UUIDとして出力
   //   例: "123e4567-e89b-12d3-a456-426614174000"
   const char *id;
} clap_universal_plugin_id_t;