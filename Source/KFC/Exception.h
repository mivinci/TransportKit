#pragma once
#include <string>

#include "KFC/Platform.h"

namespace KFC {
class Exception : std::exception {
 public:
  explicit Exception(const std::string &message,
                     const std::string &scope = "KFC") {
    info.append("[");
    info.append(scope);
    info.append("] ");
    info.append(message);
    info.append("\n Stack trace:\n");
    // TODO: stacktrace
  }

  KFC_NODISCARD const char *what() const noexcept override {
    return info.c_str();
  }

 private:
  std::string info;
};
}  // namespace KFC
