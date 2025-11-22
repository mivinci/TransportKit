#pragma once
#include "KFC/Preclude.h"
#include <cstdint>

KFC_NAMESPACE_BEG

inline uint16_t swap(const uint16_t x) { return (x << 8) | (x >> 8); }
inline uint32_t swap(const uint32_t x) {
  return ((x << 24) & 0xff000000) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) |
         ((x >> 24) & 0x000000ff);
}
inline uint64_t swap(const uint64_t x) {
  return ((x << 56) & 0xff00000000000000ULL) | ((x << 40) & 0x00ff000000000000ULL) |
         ((x << 24) & 0x0000ff0000000000ULL) | ((x << 8) & 0x000000ff00000000ULL) |
         ((x >> 8) & 0x00000000ff000000ULL) | ((x >> 24) & 0x0000000000ff0000ULL) |
         ((x >> 40) & 0x000000000000ff00ULL) | ((x >> 56) & 0x00000000000000ffULL);
}

// Returns the least significant bit position of x.
//
// Example:
//
//   CountTrailingZeros(0b00000001) // 0
//   CountTrailingZeros(0b00010000) // 4
//
int countTrailingZeros(unsigned int x);

KFC_NAMESPACE_END
