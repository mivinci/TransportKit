#pragma once
#include <functional>

namespace KFC {
template <class Func>
using ReturnType = decltype(std::declval<Func>()());
}