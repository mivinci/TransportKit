#pragma once
#include "KFC/Log.h"
#include "KFC/Platform.h"

namespace KFC {
#define KFC_CHECK(expr, ...)                                                                       \
  do {                                                                                             \
    if (expr) break;                                                                               \
    KFC_THROW("check(" #expr "): " __VA_ARGS__);                                                   \
    KFC_UNREACHABLE();                                                                             \
  } while (0)

#define KFC_ASSERT(expr, ...)                                                                      \
  do {                                                                                             \
    if (expr) break;                                                                               \
    KFC_FATAL("assert(" #expr "): " __VA_ARGS__);                                                  \
    KFC_UNREACHABLE();                                                                             \
  } while (0)

#define KFC_ASSERT_SYSCALL(expr)                                                                   \
  do {                                                                                             \
    int status = expr;                                                                             \
    if (status == 0) break;                                                                        \
    KFC_FATAL("assert_syscall(" #expr "): rc(%d)", status);                                        \
    KFC_UNREACHABLE();                                                                             \
  } while (0)

#define KFC_TODO(...)                                                                              \
  do {                                                                                             \
    KFC_FATAL("TODO: " __VA_ARGS__);                                                               \
    KFC_UNREACHABLE();                                                                             \
  } while (0)

#if __cplusplus >= 201402L
#define KFC_STATIC_ASSERT(cond, ...) static_assert(cond, __VA_ARGS__)
#else
#define KFC_STATIC_ASSERT(cond, ...)                                                               \
  struct __assert {                                                                                \
    static_assert((cond), __VA_ARGS__);                                                            \
  };                                                                                               \
  static_cast<void>(__assert())
#endif

#define KFC_STATIC_ASSERT_IS_BASE_OF(B, U)                                                         \
  KFC_STATIC_ASSERT((std::is_base_of<B, U>::value), "" #U " should be a sub-class of " #B)
} // namespace KFC
