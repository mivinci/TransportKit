#pragma once
#include "KFC/Own.h"

namespace KFC {
template <typename T> constexpr void drop(T &t) { t.~T(); }

template <class T> class Disposer {
public:
  virtual ~Disposer() noexcept(false) = default;
  virtual void dispose(T *t) const = 0;
};

} // namespace KFC
