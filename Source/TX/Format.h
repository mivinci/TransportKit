#pragma once
#include <string>
#if defined(USE_FMTLIB)
#include "fmt/format.h"
#elif __cplusplus >= 202002L && !defined(__clang__)
#include <format>
#else
#include <cstdarg>
#endif

namespace TX {
#if defined(USE_FMTLIB)
#define TX_FORMAT(format, ...) fmtlib_format(format, __VA_ARGS__)
template <class... T>
std::string TX::fmtlib_format(fmt::format_string<T...> format, T &&...args) {
  return fmt::vformat(format, fmt::make_format_args(args...));
}
#elif __cplusplus >= 202002L && !defined(__clang__)
#define TX_FORMAT(fmt, ...) (std::format(fmt, __VA_ARGS__))
#else
#define TX_FORMAT(...) (TX::legacy_format(__VA_ARGS__))
inline std::string legacy_format(const char *format, ...) {
  thread_local char buf[2048];
  va_list ap;
  va_start(ap, format);
  std::vsnprintf(buf, 2048, format, ap);
  return buf;
}
#endif
}  // namespace TX
