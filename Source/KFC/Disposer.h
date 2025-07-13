#pragma once

#include "KFC/Preclude.h"

KFC_NAMESPACE_BEG

class Disposer {
protected:
  virtual void disposePtr(void *ptr) = 0;

public:
  virtual ~Disposer() noexcept(false) = default;
  template <class T> void dispose(T *ptr) { DisposeHelper<T>::dispose(*this, ptr); }

private:
  template <class T, bool = KFC_IS_POLYMORPHIC(T)> struct DisposeHelper;

  template <class T> struct DisposeHelper<T, true> {
    static void dispose(Disposer &disposer, T *ptr) {
      disposer.disposePtr(dynamic_cast<void *>(ptr));
    }
  };

  template <class T> struct DisposeHelper<T, false> {
    static void dispose(Disposer &disposer, T *ptr) {
      disposer.disposePtr(static_cast<void *>(ptr));
    }
  };
};

template <class T> class DeleteDisposer final : public Disposer {
public:
  void disposePtr(void *ptr) override { delete static_cast<T *>(ptr); }
};

template <class T> class DestructOnlyDisposer final : public Disposer {
public:
  void disposePtr(void *ptr) override { static_cast<T *>(ptr)->~T(); }
};

KFC_NAMESPACE_END
