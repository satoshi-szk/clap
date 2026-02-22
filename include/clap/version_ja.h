#pragma once

#include "private/macros.h"
#include "private/std.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_version {
   // これはメジャーABIとAPI設計です
   // バージョン0.X.Yは開発段階に対応し、APIとABIは安定していません
   // バージョン1.X.Yはリリース段階に対応し、APIとABIは安定しています
   uint32_t major;
   uint32_t minor;
   uint32_t revision;
} clap_version_t;

#ifdef __cplusplus
}
#endif

#define CLAP_VERSION_MAJOR 1
#define CLAP_VERSION_MINOR 2
#define CLAP_VERSION_REVISION 6

#define CLAP_VERSION_INIT                                                                          \
   { (uint32_t)CLAP_VERSION_MAJOR, (uint32_t)CLAP_VERSION_MINOR, (uint32_t)CLAP_VERSION_REVISION }

#define CLAP_VERSION_LT(maj,min,rev) ((CLAP_VERSION_MAJOR < (maj)) || \
                    ((maj) == CLAP_VERSION_MAJOR && CLAP_VERSION_MINOR < (min)) || \
                    ((maj) == CLAP_VERSION_MAJOR && (min) == CLAP_VERSION_MINOR && CLAP_VERSION_REVISION < (rev)))
#define CLAP_VERSION_EQ(maj,min,rev) (((maj) == CLAP_VERSION_MAJOR) && ((min) == CLAP_VERSION_MINOR) && ((rev) == CLAP_VERSION_REVISION))
#define CLAP_VERSION_GE(maj,min,rev) (!CLAP_VERSION_LT(maj,min,rev))

static const CLAP_CONSTEXPR clap_version_t CLAP_VERSION = CLAP_VERSION_INIT;

CLAP_NODISCARD static inline CLAP_CONSTEXPR bool
clap_version_is_compatible(const clap_version_t v) {
   // バージョン0.x.yは開発段階で使用され、互換性がありません
   return v.major >= 1;
}