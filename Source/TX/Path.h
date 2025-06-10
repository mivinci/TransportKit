#pragma once
#include "TX/Platform.h"
#include "TX/String.h"

namespace TX {
class Path {
 public:
  explicit Path(const String &path) {}
  TX_NODISCARD char *ToCStr() const { return nullptr; }

 private:
};
}  // namespace TX
