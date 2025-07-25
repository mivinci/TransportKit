#pragma once

/*
 * C++98: 199711L
 * C++11: 201103L
 * C++14: 201402L
 * C++17: 201703L
 * C++20: 202002L
 * C++23: 202302L（draft）
 */
namespace TX {
#define TX_CONCAT_(x, y) x##y
#define TX_CONCAT(x, y) TX_CONCAT_(x, y)
#define TX_UNIQUE_NAME(name) TX_CONCAT(name, __LINE__)

#if defined(__GNUC__) || defined(__clang__)
#define TX_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#define TX_UNREACHABLE() __assume(0)
#else
#define TX_UNREACHABLE() ((void)0)
#endif

#if __cplusplus >= 201103L
#define TX_NORETURN [[noreturn]]
#elif defined(__GNUC__) || defined(__clang__)
#define TX_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define TX_NORETURN __declspec(noreturn)
#else
#define TX_NORETURN
#endif

#if __cplusplus >= 201703L
#define TX_NODISCARD [[nodiscard]]
#elif defined(__GNUC__) || defined(__clang__)
#define TX_NODISCARD __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
#define TX_NODISCARD _Check_return_
#else
#define TX_NODISCARD
#endif

#if __cplusplus >= 201703L
#define TX_FALLTHROUGH [[fallthrough]]
#elif defined(__GNUC__) || defined(__clang__)
#define TX_FALLTHROUGH [[fallthrough]]
#elif defined(_MSC_VER)
#define TX_FALLTHROUGH __fallthrough
#else
#define TX_FALLTHROUGH
#endif

#if __cplusplus >= 201703L
#define TX_DEPRECATED [[deprecated]]
#elif defined(__GNUC__) || defined(__clang__)
#define TX_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define TX_DEPRECATED __declspec(deprecated)
#else
#define TX_DEPRECATED
#endif

#if __cplusplus >= 201703L
#define TX_CONSTEXPR_IF(expr) if constexpr (expr)
#elif defined(__GNUC__) || defined(__clang__)
#define TX_CONSTEXPR_IF(expr) if (__builtin_constant_p(expr))
#elif defined(_MSC_VER)
#define TX_CONSTEXPR_IF(expr) if (expr)
#else
#define TX_CONSTEXPR_IF(expr) if (expr)
#endif

#if __cplusplus >= 201703L
#define TX_CONSTEXPR_FUNCTION inline
#else
#define TX_CONSTEXPR_FUNCTION constexpr
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_LIKELY(x) __builtin_expect(!!(x), 1)
#define TX_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define TX_LIKELY(x) (x)
#define TX_UNLIKELY(x) (x)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_NO_INLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define TX_NO_INLINE __declspec(noinline)
#else
#define TX_NO_INLINE
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_ALWAYS_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define TX_ALWAYS_INLINE __forceinline
#else
#define TX_ALWAYS_INLINE inline
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
#define TX_UNUSED __declspec(unused)
#else
#define TX_UNUSED
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_WEAK __attribute__((weak))
#elif defined(_MSC_VER)
#define TX_WEAK __declspec(selectany)
#else
#define TX_WEAK
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_ALIGNAS(x) __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#define TX_ALIGNAS(x) __declspec(align(x))
#else
#define TX_ALIGNAS(x)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_ALIGNOF(x) __alignof__(x)
#elif defined(_MSC_VER)
#define TX_ALIGNOF(x) __alignof(x)
#else
#define TX_ALIGNOF(x) sizeof(x)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))
#elif defined(_MSC_VER)
#define TX_ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))
#else
#define TX_ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_ALIGN_DOWN(x, align) ((x) & ~((align) - 1))
#elif defined(_MSC_VER)
#define TX_ALIGN_DOWN(x, align) ((x) & ~((align) - 1))
#else
#define TX_ALIGN_DOWN(x, align) ((x) & ~((align) - 1))
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_CACHE_LINE_SIZE 64
#elif defined(_MSC_VER)
#define TX_CACHE_LINE_SIZE 64
#else
#define TX_CACHE_LINE_SIZE 64
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_CACHE_LINE_ALIGN __attribute__((aligned(TX_CACHE_LINE_SIZE)))
#elif defined(_MSC_VER)
#define TX_CACHE_LINE_ALIGN __declspec(align(TX_CACHE_LINE_SIZE))
#else
#define TX_CACHE_LINE_ALIGN
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_NO_OPTIMIZE __attribute__((optimize("O0")))
#elif defined(_MSC_VER)
#define TX_NO_OPTIMIZE __declspec(noinline)
#else
#define TX_NO_OPTIMIZE
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TX_NO_UNROLL __attribute__((optimize("O0")))
#elif defined(_MSC_VER)
#define TX_NO_UNROLL __declspec(noinline)
#else
#define TX_NO_UNROLL
#endif

#if __GNUC__ || __clang__
#define TX_SILENCE_DANGLING_ELSE_BEGIN \
  _Pragma("GCC diagnostic push")       \
      _Pragma("GCC diagnostic ignored \"-Wdangling-else\"")
#define TX_SILENCE_DANGLING_ELSE_END _Pragma("GCC diagnostic pop")
#else
#define TX_SILENCE_DANGLING_ELSE_BEGIN
#define TX_SILENCE_DANGLING_ELSE_END
#endif

#define TX_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define TX_MAX(a, b) (((a) > (b)) ? (a) : (b))

}  // namespace TX
