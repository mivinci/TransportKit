#include "KFC/Format.h"

#include <cstdarg>

namespace KFC {
std::string threadSafeBoundedBufferFormat(const char* format, ...) {
  char buf[2048];
  va_list ap;
  va_start(ap, format);
  std::vsnprintf(buf, 2048, format, ap);
  return buf;
}

}  // namespace KFC