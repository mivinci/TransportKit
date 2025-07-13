#pragma once

#define KFC_DISALLOW_COPY(T)                                                                       \
  T(const T &) = delete;                                                                           \
  T &operator=(const T &) = delete;

#define KFC_DISALLOW_MOVE(T)                                                                       \
  T(T &&) = delete;                                                                                \
  T &operator=(T &&) = delete;

#define KFC_DISALLOW_COPY_AND_MOVE(T)                                                              \
  KFC_DISALLOW_COPY(T)                                                                             \
  KFC_DISALLOW_MOVE(T)

#define KFC_DEFAULT_MOVE(T)                                                                        \
  T(T &&) = default;                                                                               \
  T &operator=(T &&) = default;

#define KFC_COPY(t) (t)
#define KFC_MOVE(t) (std::move(t))
