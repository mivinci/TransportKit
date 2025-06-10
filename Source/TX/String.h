#pragma once
#include <string>

#include "TX/Platform.h"

namespace TX {
using String = std::string;
#if __cplusplus >= 201703L
using StringView = std::string_view;
#else
#endif
}  // namespace TX
