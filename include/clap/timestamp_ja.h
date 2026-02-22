#pragma once

#include "private/std.h"
#include "private/macros.h"

// この型はタイムスタンプを定義します: UNIX EPOCHからの秒数。
// Cのtime_t time(time_t *)を参照してください。
typedef uint64_t clap_timestamp;

// 不明なタイムスタンプの値。
static const CLAP_CONSTEXPR clap_timestamp CLAP_TIMESTAMP_UNKNOWN = 0;