#include "KFC/Exception.h"

namespace KFC {

Exception::Exception() {}

Exception::Exception(const char *file, int line, const String &message) {}

const char *Exception::what() const noexcept { return "TODO"; }

void throwFatalException(Exception &&e, int ignore) {
  // TODO
  abort();
}

} // namespace KFC
