#pragma once
#include <string>

#include "TX/Platform.h"

namespace TX {
class Exception : std::exception {
 public:
  explicit Exception(const std::string &message,
                     const std::string &scope = "TX") {
    info.append("[");
    info.append(scope);
    info.append("] ");
    info.append(message);
    info.append("\n Stack trace:\n");
    // TODO: stacktrace
  }

  TX_NODISCARD const char *what() const noexcept override {
    return info.c_str();
  }

 private:
  std::string info;
};
}  // namespace TX
