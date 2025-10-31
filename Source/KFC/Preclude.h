#pragma once
#include <cstddef>

#define KFC_NAMESPACE KFC
#define KFC_NAMESPACE_BEG namespace KFC_NAMESPACE {
#define KFC_NAMESPACE_END }

/*
 * C++98: 199711L
 * C++11: 201103L
 * C++14: 201402L
 * C++17: 201703L
 * C++20: 202002L
 * C++23: 202302L（draft）
 */

#define KFC_CONCAT_(x, y) x##y
#define KFC_CONCAT(x, y) KFC_CONCAT_(x, y)
#define KFC_UNIQUE_NAME(name) KFC_CONCAT(name, __LINE__)

#if defined(__GNUC__) || defined(__clang__)
#define KFC_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#define KFC_UNREACHABLE() __assume(0)
#else
#define KFC_UNREACHABLE() ((void)0)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define KFC_NOINLINE __declspec(noinline)
#else
#define KFC_NOINLINE
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define KFC_NORETURN __declspec(noreturn)
#else
#define KFC_NORETURN
#endif

#if __cplusplus >= 201703L
#define KFC_NODISCARD [[nodiscard]]
#elif defined(__GNUC__) || defined(__clang__)
#define KFC_NODISCARD __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
#define KFC_NODISCARD _Check_return_
#else
#define KFC_NODISCARD
#endif

#if __cplusplus >= 201703L
#define KFC_FALLTHROUGH [[fallthrough]]
#elif defined(__GNUC__) || defined(__clang__)
#define KFC_FALLTHROUGH [[fallthrough]]
#elif defined(_MSC_VER)
#define KFC_FALLTHROUGH __fallthrough
#else
#define KFC_FALLTHROUGH
#endif

#if __cplusplus >= 201703L
#define KFC_DEPRECATED [[deprecated]]
#elif defined(__GNUC__) || defined(__clang__)
#define KFC_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define KFC_DEPRECATED __declspec(deprecated)
#else
#define KFC_DEPRECATED
#endif

#if __cplusplus >= 201703L
#define KFC_CONSTEXPR_IF(expr) if constexpr (expr)
#elif defined(__GNUC__) || defined(__clang__)
#define KFC_CONSTEXPR_IF(expr) if (__builtin_constant_p(expr))
#elif defined(_MSC_VER)
#define KFC_CONSTEXPR_IF(expr) if (expr)
#else
#define KFC_CONSTEXPR_IF(expr) if (expr)
#endif

#if __cplusplus >= 201703L
#include <type_traits>
#define KFC_IS_POLYMORPHIC(T) std::is_polymorphic<T>::value
#else
#if defined(_MSC_VER)
#define KFC_IS_POLYMORPHIC(T) __is_polymorphic_class(T)
#elif defined(__GNUC__) || defined(__clang__)
#define KFC_IS_POLYMORPHIC(T) __is_polymorphic(T)
#else
#define KFC_IS_POLYMORPHIC(T) false
#endif
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_LIKELY(x) __builtin_expect(!!(x), 1)
#define KFC_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define KFC_LIKELY(x) (x)
#define KFC_UNLIKELY(x) (x)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_ALWAYS_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define KFC_ALWAYS_INLINE __forceinline
#else
#define KFC_ALWAYS_INLINE inline
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
#define KFC_UNUSED __declspec(unused)
#else
#define KFC_UNUSED
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_WEAK __attribute__((weak))
#elif defined(_MSC_VER)
#define KFC_WEAK __declspec(selectany)
#else
#define KFC_WEAK
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_ALIGNAS(x) __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#define KFC_ALIGNAS(x) __declspec(align(x))
#else
#define KFC_ALIGNAS(x)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_ALIGNOF(x) __alignof__(x)
#elif defined(_MSC_VER)
#define KFC_ALIGNOF(x) __alignof(x)
#else
#define KFC_ALIGNOF(x) sizeof(x)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))
#elif defined(_MSC_VER)
#define KFC_ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))
#else
#define KFC_ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_ALIGN_DOWN(x, align) ((x) & ~((align) - 1))
#elif defined(_MSC_VER)
#define KFC_ALIGN_DOWN(x, align) ((x) & ~((align) - 1))
#else
#define KFC_ALIGN_DOWN(x, align) ((x) & ~((align) - 1))
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_CACHE_LINE_SIZE 64
#elif defined(_MSC_VER)
#define KFC_CACHE_LINE_SIZE 64
#else
#define KFC_CACHE_LINE_SIZE 64
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_CACHE_LINE_ALIGN __attribute__((aligned(KFC_CACHE_LINE_SIZE)))
#elif defined(_MSC_VER)
#define KFC_CACHE_LINE_ALIGN __declspec(align(KFC_CACHE_LINE_SIZE))
#else
#define KFC_CACHE_LINE_ALIGN
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_NO_OPTIMIZE __attribute__((optimize("O0")))
#elif defined(_MSC_VER)
#define KFC_NO_OPTIMIZE __declspec(noinline)
#else
#define KFC_NO_OPTIMIZE
#endif

#if defined(__GNUC__) || defined(__clang__)
#define KFC_NO_UNROLL __attribute__((optimize("O0")))
#elif defined(_MSC_VER)
#define KFC_NO_UNROLL __declspec(noinline)
#else
#define KFC_NO_UNROLL
#endif

#if __GNUC__ || __clang__
#define KFC_SILENCE_DANGLING_ELSE_BEGIN                                                            \
  _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdangling-else\"")
#define KFC_SILENCE_DANGLING_ELSE_END _Pragma("GCC diagnostic pop")
#else
#define KFC_SILENCE_DANGLING_ELSE_BEGIN
#define KFC_SILENCE_DANGLING_ELSE_END
#endif

#define KFC_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define KFC_MAX(a, b) (((a) > (b)) ? (a) : (b))

#define KFC_DISCARD(x) (void)(x)
