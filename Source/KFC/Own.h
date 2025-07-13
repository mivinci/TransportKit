#pragma once

#include "KFC/Exchange.h"
#include "KFC/Memory.h"

namespace KFC {
// Own is a simple ownership wrapper that takes sole ownership of a raw pointer. It ensures that the
// owned object is deleted when the Own object goes out of scope, and does not support copying to
// avoid ownership ambiguity. Use this class when you need explicit ownership semantics without
// reference counting.
// TODO: custom disposer
template <class T> class Own {
public:
  explicit constexpr Own() : m_ptr(nullptr) {}
  explicit constexpr Own(T *t) : m_ptr(t) {}

  template <class U> Own(Own<U> &&other) : m_ptr(other.m_ptr) { // NOLINT(*-explicit-constructor)
    KFC_STATIC_ASSERT_IS_BASE_OF(T, U);
    other.m_ptr = nullptr;
  }

  Own(Own &&other) noexcept {
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
  }

  ~Own() { dispose(); }
  KFC_DISALLOW_COPY(Own)

  Own &operator=(std::nullptr_t) {
    dispose();
    return *this;
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

  template <class U> Own<U> downcast() {
    KFC_STATIC_ASSERT_IS_BASE_OF(T, U);
    auto ptr = static_cast<U *>(m_ptr);
    m_ptr = nullptr;
    return Own<U>(ptr);
  }

  T *take() { return KFC_EXCHANGE(m_ptr, nullptr); }

  void reset(T *t = nullptr) {
    if (m_ptr != t) {
      delete m_ptr;
      m_ptr = t;
    }
  }

  void dispose() {
    if (!m_ptr) return;
    delete m_ptr;
    m_ptr = nullptr;
  }

  explicit operator bool() const { return !!m_ptr; }

  T &operator*() { return *m_ptr; }
  T *operator->() { return m_ptr; }

private:
  template <class> friend class Own;

  T *m_ptr;
};
} // namespace KFC
