#pragma once

#include "private/std.h"
#include "private/macros.h"

/// ビート時間と秒時間の固定小数点表現を使用します
/// 使用法:
///   double x = ...; // ビート単位
///   clap_beattime y = round(CLAP_BEATTIME_FACTOR * x);

// これは変更されません
static const CLAP_CONSTEXPR int64_t CLAP_BEATTIME_FACTOR = 1LL << 31;
static const CLAP_CONSTEXPR int64_t CLAP_SECTIME_FACTOR = 1LL << 31;

typedef int64_t clap_beattime;
typedef int64_t clap_sectime;