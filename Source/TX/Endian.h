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

inline bool IsBigEndian() {
  union {
    uint32 i;
    uint8 b[4];
  } u = {0x01020304};
  return u.b[0] == 1;
}
}  // namespace TX
