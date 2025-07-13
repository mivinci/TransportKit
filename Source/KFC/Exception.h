#pragma once
#include "KFC/Platform.h"
#include "KFC/String.h"

namespace KFC {
class Exception : std::exception {
 public:
  explicit Exception(const String &message, const String &scope = "KFC") {
    info.append("[");
    info.append(scope);
    info.append("] ");
    info.append(message);
    info.append("\n Stack trace:\n");
    // TODO: stacktrace
  }

  KFC_NODISCARD const char *what() const noexcept override { return info.c_str(); }

 private:
  String info;
};
}  // namespace KFC
