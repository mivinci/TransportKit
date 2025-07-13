#pragma once

#include "TX/Bits.h"

namespace TX {
#if __cplusplus >= 202002L
#define TX_BIG_ENDIAN (std::endian::native == std::endian::big)
#define TX_LITTLE_ENDIAN (std::endian::native == std::endian::little)
#else
#define TX_BIG_ENDIAN IsBigEndian()
#define TX_LITTLE_ENDIAN (!TX_BIG_ENDIAN)
#endif

constexpr bool IsBigEndian() {
  return static_cast<uint32_t>(0x01020304) >> 24 == 1;
}
}  // namespace TX
