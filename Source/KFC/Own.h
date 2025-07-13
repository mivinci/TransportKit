#pragma once

#include "KFC/CopyMove.h"
#include "KFC/Exchange.h"

#if __cplusplus >= 201703L
#define KFC_STATIC_ASSERT_HAS_DISPOSE(D)                                                           \
  static_assert(std::is_invocable_v<decltype(&D::dispose), const typename D::ValueType *>,         \
                "Disposer must implement static dispose method")
#else
#define KFC_STATIC_ASSERT_HAS_DISPOSE(D)
#endif

namespace KFC {
// A disposer that simply disposes an object by `delete`.
template <class T> struct DeleteDisposer {
  using ValueType = T;
  static void dispose(const ValueType *t) { delete t; }
};

// Own is an ownership wrapper that takes sole ownership of a raw pointer. It ensures that the
// owned object is disposed when the Own object goes out of scope, and does not support copying to
// avoid ownership ambiguity. Use this class when you need explicit ownership semantics without
// reference counting.
template <class T, class StaticDisposer> class Own {
  KFC_STATIC_ASSERT_HAS_DISPOSE(StaticDisposer);

public:
  KFC_DISALLOW_COPY(Own)
  constexpr Own() : m_ptr(nullptr) {}
  constexpr Own(T *t) : m_ptr(t) {} // NOLINT(*-explicit-constructor)
  template <class U>
  Own(Own<U, StaticDisposer> &&other) : m_ptr(other.m_ptr) { // NOLINT(*-explicit-constructor)
    KFC_STATIC_ASSERT_IS_BASE_OF(T, U);
    other.m_ptr = nullptr;
  }

  Own(Own &&other) noexcept {
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
  }

  ~Own() noexcept(false) { dispose(); }

  Own &operator=(std::nullptr_t) {
    dispose();
    return *this;
  }

  Own &operator=(Own &&other) noexcept {
    dispose();
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
    return *this;
  }

  T &operator*() { return *m_ptr; }
  T *operator->() { return m_ptr; }

  explicit operator bool() const { return !!m_ptr; }

  // Takes the raw pointer out of the Own object without disposing it.
  T *take() { return KFC_EXCHANGE(m_ptr, nullptr); }

  // Resets the raw pointer of the Own object to `t`, disposing the previous one if it is different.
  void reset(T *t = nullptr) {
    if (m_ptr != t) {
      dispose();
      m_ptr = t;
    }
  }

  // Downcasts the owned object to a subclass.
  template <class U> Own<U, StaticDisposer> downcast() {
    KFC_STATIC_ASSERT_IS_BASE_OF(T, U);
    if (!m_ptr) return Own<U, StaticDisposer>(nullptr);
    auto ptr = static_cast<U *>(m_ptr);
    m_ptr = nullptr;
    return Own<U, StaticDisposer>(ptr);
  }

private:
  // Disposes the owned object.
  void dispose() {
    if (!m_ptr) return;
    StaticDisposer::dispose(m_ptr);
    m_ptr = nullptr;
  }

  T *m_ptr;

  template <class, class> friend class Own;
};

template <class T> class Own {};
} // namespace KFC