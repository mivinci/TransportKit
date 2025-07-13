#pragma once

#include "KFC/CopyMove.h"
#include "KFC/Exchange.h"
#include "KFC/Memory.h"

#if __cplusplus >= 201703L
#define KFC_STATIC_ASSERT_HAS_DISPOSE(D)                                                           \
  static_assert(std::is_invocable_v<decltype(&D::dispose), const typename D::ValueType *>,         \
                "Disposer must implement static dispose method")
#else
#define KFC_STATIC_ASSERT_HAS_DISPOSE(D)
#endif

namespace KFC {
template <class T> struct DeleteStaticDisposer {
  // A disposer that simply disposes an object by `delete`.
  using ValueType = T;
  static void dispose(const ValueType *t) { delete t; }
};

template <class T, class StaticDisposer = std::nullptr_t> class Own {
  // Own is an ownership wrapper that takes sole ownership of a raw pointer. It ensures that the
  // owned object is disposed when the Own object goes out of scope, and does not support copying to
  // avoid ownership ambiguity. Use this class when you need explicit ownership semantics without
  // reference counting.
  KFC_STATIC_ASSERT_HAS_DISPOSE(StaticDisposer);

public:
  KFC_DISALLOW_COPY(Own)
  constexpr Own() : m_ptr(nullptr) {}
  constexpr Own(T *ptr) : m_ptr(ptr) {}

  template <class U> Own(Own<U, StaticDisposer> &&other) : m_ptr(other.m_ptr) {
    // KFC_STATIC_ASSERT_IS_BASE_OF(T, U);
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

  T *take() {
    // Takes the raw pointer out of the Own object without disposing it.
    return KFC_EXCHANGE(m_ptr, nullptr);
  }

  void reset(T *ptr = nullptr) {
    // Resets the raw pointer of the Own object to `t`, disposing the previous one if it is
    // different.
    if (m_ptr != ptr) {
      dispose();
      m_ptr = ptr;
    }
  }

  // template <class U> Own<U, StaticDisposer> downcast() {
  //   // Downcasts the owned object to a subclass.
  //   KFC_STATIC_ASSERT_IS_BASE_OF(T, U);
  //   Own<U, StaticDisposer> sub;
  //   if (m_ptr) {
  //     sub.m_ptr = static_cast<U *>(m_ptr);
  //     m_ptr = nullptr;
  //   }
  //   return sub;
  // }

private:
  void dispose() {
    if (T *ptrCopy = m_ptr) {
      // Makes sure that event if `dispose` throws an exception, the pointer is set to nullptr. But
      // if `dispose` throws before actually freeing the object, there will be a memory leak.
      m_ptr = nullptr;
      StaticDisposer::dispose(ptrCopy);
    }
  }

  T *m_ptr;

  template <class, class> friend class Own;
};

template <class T> class Own<T, std::nullptr_t> {
  // Specialization of `Own<T, StaticDisposer>` that disposes owned object by dynamic call to the
  // disposer than by static call.
public:
  KFC_DISALLOW_COPY(Own)
  constexpr Own() : m_ptr(nullptr), m_disposer(nullptr) {}
  constexpr Own(T *ptr, Disposer *disposer) : m_ptr(ptr), m_disposer(disposer) {}
  constexpr Own(Own &&other) noexcept : m_ptr(other.m_ptr), m_disposer(other.m_disposer) {
    other.m_ptr = nullptr;
  }

  template <class U>
  constexpr Own(Own<U> &&other) : m_ptr(other.m_ptr), m_disposer(other.m_disposer) {
    // KFC_STATIC_ASSERT_IS_BASE_OF(T, U);
    other.m_ptr = nullptr;
  }

  template <class U> constexpr Own(U *ptr, Disposer *disposer) : m_ptr(ptr), m_disposer(disposer) {
    // KFC_STATIC_ASSERT_IS_BASE_OF(T, U);
  }

  ~Own() noexcept(false) { dispose(); }

  Own &operator=(std::nullptr_t) {
    dispose();
    return *this;
  }

  Own &operator=(Own &&other) noexcept {
    dispose();
    m_ptr = other.m_ptr;
    m_disposer = other.m_disposer;
    other.m_ptr = nullptr;
    return *this;
  }

  T &operator*() { return *m_ptr; }
  T *operator->() { return m_ptr; }

  explicit operator bool() const { return !!m_ptr; }

private:
  void dispose() {
    if (T *ptrCopy = m_ptr) {
      m_ptr = nullptr;
      if (KFC_LIKELY(m_disposer)) {
        m_disposer->dispose(ptrCopy);
      }
    }
  }

  T *m_ptr;
  Disposer *m_disposer;

  template <class, class> friend class Own;
};

} // namespace KFC