#pragma once
#include "KFC/String.h"
#if defined(USE_FMT)
#include "fmt/format.h"
#endif

namespace KFC {
#if defined(USE_FMT)
#define KFC_FORMAT(format, ...) fmtlib_format(format, __VA_ARGS__)
template <class... T> std::string KFC::fmtlib_format(fmt::format_string<T...> format, T &&...args) {
  return fmt::vformat(format, fmt::make_format_args(args...));
}
#else
#define KFC_FORMAT(...) (KFC::threadSafeBoundedBufferFormat(__VA_ARGS__))
std::string threadSafeBoundedBufferFormat(const char *format = "", ...);
#endif
} // namespace KFC
