#pragma once
#include "KFC/Exception.h"
#include "KFC/Preclude.h"

namespace KFC {
#define KFC_CHECK(expr, ...)                                                                       \
  do {                                                                                             \
    if (!!(expr)) break;                                                                           \
    KFC_THROW_FATAL((KFC_NAMESPACE::Exception::Kind::Logic), __VA_ARGS__);                         \
    KFC_UNREACHABLE();                                                                             \
  } while (0)

#define KFC_CHECK_SYSCALL(expr)                                                                    \
  do {                                                                                             \
    int rc = expr;                                                                                 \
    if (rc >= 0) break;                                                                            \
    KFC_THROW_FATAL((KFC_NAMESPACE::Exception::Kind::Syscall),                                     \
                    "syscall(" #expr ") returned %d, errno: %d", rc, errno);                       \
    KFC_UNREACHABLE();                                                                             \
  } while (0)

// TODO
#define KFC_ASSERT(expr, ...)

#define KFC_TODO(...)                                                                              \
  do {                                                                                             \
    KFC_THROW_FATAL(KFC_NAMESPACE::kUnImplementedError, "TODO: " __VA_ARGS__);                     \
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
