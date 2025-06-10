#pragma once
#include <functional>

namespace TX {
template <class Func>
using ReturnType = decltype(std::declval<Func>()());
}