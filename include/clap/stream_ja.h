#pragma once

#include "private/std.h"
#include "private/macros.h"

/// @page Streams
///
/// ## ストリーム使用に関する注意
///
/// 状態のロードと保存のために`clap_istream`と`clap_ostream`オブジェクトを使用する際は、
/// ホストが一度に読み書きできるバイト数を制限する可能性があることを
/// 心に留めておくことが重要です。ストリームの読み書き関数の戻り値は、
/// 実際に読み書きされたバイト数を示します。状態全体を確実に読み書きするためには、
/// ループを使用する必要があります。ファイル終端とIOエラーコードの
/// 負の戻り値も考慮することを忘れないでください。

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_istream {
   void *ctx; // ストリーム用の予約ポインター

   // 読み取ったバイト数を返します; 0はファイル終端を示し、-1は読み取りエラーを示します
   int64_t(CLAP_ABI *read)(const struct clap_istream *stream, void *buffer, uint64_t size);
} clap_istream_t;

typedef struct clap_ostream {
   void *ctx; // ストリーム用の予約ポインター

   // 書き込んだバイト数を返します; 書き込みエラーで-1
   int64_t(CLAP_ABI *write)(const struct clap_ostream *stream, const void *buffer, uint64_t size);
} clap_ostream_t;

#ifdef __cplusplus
}
#endif