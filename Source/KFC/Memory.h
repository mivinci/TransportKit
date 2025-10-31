#pragma once

#include "KFC/Disposer.h"
#include <utility>

KFC_NAMESPACE_BEG

template <class... T> void doAll(T...) {}

template <typename T, size_t size> constexpr size_t sizeOf(T (&arr)[size]) { return size; }
template <typename T> constexpr size_t sizeOf(T &&arr) { return arr.size(); }

template <class T> void dtor(T &t) { t.~T(); }
template <class T, class... Args> void ctor(T &t, Args &&...args) {
  new (&t) T(std::forward<Args>(args)...);
}

KFC_NAMESPACE_END
