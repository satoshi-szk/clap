#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // ユーザーに表示可能な名前の文字列容量。
   CLAP_NAME_SIZE = 256,

   // モジュール階層内のパラメータやネストされたトラックグループセット内のパスなど、
   // パスを記述するための文字列容量。
   //
   // これは、NTFSが最大32Kの長いパスを許可するため、
   // ディスク上のファイルパスを記述するのには適していません。
   CLAP_PATH_SIZE = 1024,
};

#ifdef __cplusplus
}
#endif