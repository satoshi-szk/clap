#pragma once

#include "private/std.h"

#ifdef __cplusplus
extern "C" {
#endif

// ステレオバッファ読み取りのサンプルコード:
//
// bool isLeftConstant = (buffer->constant_mask & (1 << 0)) != 0;
// bool isRightConstant = (buffer->constant_mask & (1 << 1)) != 0;
//
// for (int i = 0; i < N; ++i) {
//    float l = data32[0][isLeftConstant ? 0 : i];
//    float r = data32[1][isRightConstant ? 0 : i];
// }
//
// 注記: constant_maskのチェックはオプションですが、
// これはバッファが定数値で満たされている必要があることを意味します。
// 理由: バッファリーダーがconstant_maskをチェックしない場合、
// ガベージサンプルを処理する可能性があり、その結果、
// ガベージサンプルがオーディオインターフェースに送信されて
// 悪い結果をもたらす可能性があります。
//
// constant_maskはヒントです。
typedef struct clap_audio_buffer {
   // data32またはdata64ポインタのいずれかが設定されます。
   float  **data32;
   double **data64;
   uint32_t channel_count;
   uint32_t latency; // オーディオインターフェースからの/への遅延
   uint64_t constant_mask;
} clap_audio_buffer_t;

#ifdef __cplusplus
}
#endif