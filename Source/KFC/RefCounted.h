#pragma once

#include <atomic>

#include "KFC/CopyMove.h"

namespace KFC {
template <class T, bool Atomic = false> class RefCounted {
public:
  using Type = typename std::conditional<Atomic, std::atomic<unsigned>, unsigned>::type;
  explicit RefCounted() : m_refCount(0), m_adopted(false) {}
  virtual ~RefCounted() noexcept(false) = default;
  void ref() { ++m_refCount; }
  void deref() {
    if (!--m_refCount) delete const_cast<T *>(static_cast<const T *>(this));
  }
  KFC_NODISCARD unsigned refCount() const { return m_refCount; }
  KFC_DISALLOW_COPY(RefCounted)

  static void adopted(RefCounted *p) {
    if (p->m_adopted) return;
    p->m_adopted = true;
  }

private:
  Type m_refCount;
  bool m_adopted;
};

template <class T> using AtomicRefCounted = RefCounted<T, true>;

#define KFC_IS_BASE_OF_REF_COUNTED(T)                                                              \
  ((std::is_base_of_v<RefCounted<T>, T>) || (std::is_base_of_v<RefCounted<T, true>, T>))

#define KFC_STATIC_ASSERT_IS_BASE_OF_REF_COUNTED(T)                                                \
  static_assert(KFC_IS_BASE_OF_REF_COUNTED(T), "T must be a subclass of RefCounted")

template <class T> T *ref(T *t) {
  if (t) t->ref();
  return t;
}

template <class T> void deref(T *t) {
  if (t) t->deref();
}
} // namespace KFC
