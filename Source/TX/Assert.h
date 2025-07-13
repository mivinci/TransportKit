#pragma once
#include "TX/Log.h"
#include "TX/Platform.h"

namespace TX {
#define TX_CHECK(expr, ...)                     \
  do {                                          \
    if (expr) break;                            \
    TX_THROW("check(" #expr "): " __VA_ARGS__); \
    TX_UNREACHABLE();                           \
  } while (0)

#define TX_ASSERT(expr, ...)                     \
  do {                                           \
    if (expr) break;                             \
    TX_FATAL("assert(" #expr "): " __VA_ARGS__); \
    TX_UNREACHABLE();                            \
  } while (0)

#define TX_ASSERT_SYSCALL(expr)                            \
  do {                                                     \
    int status = expr;                                     \
    if (status == 0) break;                                \
    TX_FATAL("assert_syscall(" #expr "): rc(%d)", status); \
    TX_UNREACHABLE();                                      \
  } while (0)

#define TX_TODO(...)                \
  do {                              \
    TX_FATAL("TODO: " __VA_ARGS__); \
    TX_UNREACHABLE();               \
  } while (0)

#if __cplusplus >= 201402L
#define TX_STATIC_ASSERT(cond, ...) static_assert(cond, __VA_ARGS__)
#else
#define TX_STATIC_ASSERT(cond, ...)     \
  struct __assert {                     \
    static_assert((cond), __VA_ARGS__); \
  };                                    \
  static_cast<void>(__assert())
#endif

#define TX_STATIC_ASSERT_IS_BASE_OF(B, U)          \
  TX_STATIC_ASSERT((std::is_base_of<B, U>::value), \
                   "" #U " should be a sub-class of " #B)
}  // namespace TX
