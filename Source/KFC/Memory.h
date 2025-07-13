#pragma once

#include "KFC/Preclude.h"

KFC_NAMESPACE_BEG

template <typename T> void ctor(T &t) { new (&t) T; }

template <typename T> void dtor(T &t) { t.~T(); }

template <class T> class Disposer {
public:
  virtual ~Disposer() noexcept(false) = default;
  virtual void dispose(T *ptr) const = 0;
};

template <class T> class DeleteDisposer final : public Disposer<T> {
public:
  constexpr void dispose(T *ptr) const override { delete ptr; }
};

template <class T> class DestructOnlyDisposer final : public Disposer<T> {
public:
  constexpr void dispose(T *ptr) const override { drop(*ptr); }
};

KFC_NAMESPACE_END
