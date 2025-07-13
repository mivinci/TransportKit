#pragma once

#include "KFC/Bits.h"

namespace KFC {
#if __cplusplus >= 202002L
#define KFC_BIG_ENDIAN (std::endian::native == std::endian::big)
#define KFC_LITTLE_ENDIAN (std::endian::native == std::endian::little)
#else
#define KFC_BIG_ENDIAN IsBigEndian()
#define KFC_LITTLE_ENDIAN (!KFC_BIG_ENDIAN)
#endif

constexpr bool IsBigEndian() {
  return static_cast<uint32_t>(0x01020304) >> 24 == 1;
}
}  // namespace KFC
