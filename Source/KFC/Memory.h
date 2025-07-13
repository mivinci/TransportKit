#pragma once

#include "KFC/Disposer.h"

KFC_NAMESPACE_BEG

template <class T, class... Args> void ctor(T &t, Args &&...args) {
  new (&t) T(std::forward<Args>(args)...);
}

template <class T> void dtor(T &t) { t.~T(); }

KFC_NAMESPACE_END
