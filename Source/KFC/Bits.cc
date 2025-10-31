#include "KFC/Bits.h"
#include "KFC/Preclude.h"

#ifdef _WIN32
#include <Windows.h>
#endif

KFC_NAMESPACE_BEG

int countTrailingZeros(unsigned int x) {
  if (x == 0) {
    return 32;
  }

#if defined(__GNUC__) || defined(__clang__)
  return __builtin_ctz(x);
#elif defined(_MSC_VER)
  DWORD n;
  return _BitScanForward(&n, x) ? n : 32;
#else
  int n = 0;
  while ((x & 1) == 0) {
    n++;
    x >>= 1;
  }
  return n;
#endif
}

KFC_NAMESPACE_END