#pragma once
#include <cstdint>

namespace TX {
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

class uint128 {
 public:
  uint128(const uint64 hi = 0, const uint64 lo = 0) : hi_(hi), lo_(lo) {}
  uint128(const int64 hi = 0, const int64 lo = 0) : hi_(hi), lo_(lo) {}
  uint128(const uint64 n) : hi_(0), lo_(n) {}
  uint128(const uint32 n) : hi_(0), lo_(n) {}
  uint128(const uint16 n) : hi_(0), lo_(n) {}
  uint128(const uint8 n) : hi_(0), lo_(n) {}
  uint128(const int64 n) : hi_(0), lo_(n) {}
  uint128(const int32 n) : hi_(0), lo_(n) {}
  uint128(const int16 n) : hi_(0), lo_(n) {}
  uint128(const int8 n) : hi_(0), lo_(n) {}

  static uint128 FromBigEndianBytes(const uint8 *bytes) { return 0; }
  void ToBigEndianBytes(uint8 *bytes) const {}

 private:
  uint64 hi_;
  uint64 lo_;
};

inline uint16 swap(const uint16 x) { return (x << 8) | (x >> 8); }
inline uint32 swap(const uint32 x) {
  return ((x << 24) & 0xff000000) | ((x << 8) & 0x00ff0000) |
         ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff);
}
inline uint64 swap(const uint64 x) {
  return ((x << 56) & 0xff00000000000000ULL) |
         ((x << 40) & 0x00ff000000000000ULL) |
         ((x << 24) & 0x0000ff0000000000ULL) |
         ((x << 8) & 0x000000ff00000000ULL) |
         ((x >> 8) & 0x00000000ff000000ULL) |
         ((x >> 24) & 0x0000000000ff0000ULL) |
         ((x >> 40) & 0x000000000000ff00ULL) |
         ((x >> 56) & 0x00000000000000ffULL);
}
}  // namespace TX
