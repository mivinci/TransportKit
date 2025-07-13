#pragma once
namespace TX {
#define TX_DISALLOW_COPY(T) \
  T(const T &) = delete;    \
  T &operator=(const T &) = delete;

#define TX_DEFAULT_MOVE(T) \
  T(T &&) = default;       \
  T &operator=(T &&) = default;

#define TX_COPY(t) (t)
#define TX_MOVE(t) (std::move(t))

template <typename T>
void drop(T &t) {
  t.~T();
}
}  // namespace TX
