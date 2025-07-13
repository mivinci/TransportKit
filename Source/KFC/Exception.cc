#include "KFC/Exception.h"

namespace KFC {

Exception::Exception() {}

Exception::Exception(const String &message, const String &scope) {}

const char *Exception::what() const noexcept { return "TODO"; }

} // namespace KFC
