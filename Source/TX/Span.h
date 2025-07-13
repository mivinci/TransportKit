#pragma once

namespace TX {
#if __cplusplus >= 202002L
#include <span>
template <class T>
using Span = std::span<T>;
#else
template <class T>
class Span { /* TODO */ };
#endif
}
