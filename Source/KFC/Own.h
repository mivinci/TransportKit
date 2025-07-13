#pragma once

#include "KFC/Exchange.h"
#include "KFC/Memory.h"

namespace KFC {
// Own is a simple ownership wrapper that takes sole ownership of a raw pointer.
// It ensures that the owned object is deleted when the Own object goes out of scope,
// and does not support copying to avoid ownership ambiguity.
// Use this class when you need explicit ownership semantics without reference counting.
template <class T>
class Own {
 public:
  explicit Own() : m_ptr(nullptr) {}
  explicit Own(T *t) : m_ptr(t) {}

  Own(Own &&other) noexcept {
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
  }

  Own &operator=(T *t) {
    m_ptr = t;
    return *this;
  }

  Own &operator=(Own &&other) noexcept {
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
    return *this;
  }

  ~Own() {
    delete m_ptr;
    m_ptr = nullptr;
  }

  KFC_DISALLOW_COPY(Own)

  T *take() { return KFC_EXCHANGE(m_ptr, nullptr); }

  void reset(T *t = nullptr) {
    if (m_ptr != t) {
      delete m_ptr;
      m_ptr = t;
    }
  }

  explicit operator bool() const { return !!m_ptr; }

  T &operator*() { return *m_ptr; }
  T *operator->() { return m_ptr; }

 private:
  T *m_ptr;
};
}  // namespace KFC
