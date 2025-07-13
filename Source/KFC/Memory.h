#pragma once
namespace KFC {
#define KFC_DISALLOW_COPY(T) \
  T(const T &) = delete;    \
  T &operator=(const T &) = delete;

#define KFC_DEFAULT_MOVE(T) \
  T(T &&) = default;       \
  T &operator=(T &&) = default;

#define KFC_COPY(t) (t)
#define KFC_MOVE(t) (std::move(t))

template <typename T>
void drop(T &t) {
  t.~T();
}
}  // namespace KFC
