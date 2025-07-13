#pragma once
#include <atomic>

#include "TX/Assert.h"
#include "TX/Exchange.h"
#include "TX/Memory.h"

namespace TX {
template <class T, bool Atomic = false>
class RefCounted {
 public:
  using Type =
      typename std::conditional<Atomic, std::atomic<unsigned>, unsigned>::type;
  explicit RefCounted() : m_refCount(0), m_adopted(false) {}
  virtual ~RefCounted() = default;
  void ref() { ++m_refCount; }
  void deref() {
    if (!--m_refCount) delete const_cast<T *>(static_cast<const T *>(this));
  }
  TX_NODISCARD unsigned refCount() const { return m_refCount; }
  TX_DISALLOW_COPY(RefCounted)

  static void adopted(RefCounted *p) {
    if (p->m_adopted) return;
    p->m_adopted = true;
  }

 private:
  Type m_refCount;
  bool m_adopted;
};

template <class T>
using AtomicRefCounted = RefCounted<T, true>;

#define TX_IS_BASE_OF_REF_COUNTED(T)        \
  ((std::is_base_of_v<RefCounted<T>, T>) || \
   (std::is_base_of_v<RefCounted<T, true>, T>))

#define TX_STATIC_ASSERT_IS_BASE_OF_REF_COUNTED(T) \
  static_assert(TX_IS_BASE_OF_REF_COUNTED(T),      \
                "T must be a subclass of RefCounted")

template <class T>
T *ref(T *t) {
  if (t) t->ref();
  return t;
}

template <class T>
void deref(T *t) {
  if (t) t->deref();
}

// Guarantees that T is non-null unless moved and does automatic reference count
// management. Unlike std::shared_ptr<T>, TX::Ref<T> is exactly the size of T*
// that is 8-byte on 64-bit systems and 4-byte on 32-bit systems. T must be a
// subclass of RefCounted.
template <class T>
class Ref final {
 public:
  explicit Ref(T &ptr) : m_ptr(ref(&ptr)) {}
  Ref(const Ref &other) : m_ptr(ref(other.ptr())) {}
  Ref(Ref &&other) noexcept : m_ptr(&other.leakRef()) { TX_ASSERT(m_ptr); }
  ~Ref() { deref(TX_EXCHANGE(m_ptr, nullptr)); }

  template <class U>
  Ref(const Ref<U> &other) : m_ptr(ref(other.ptr())) {}

  template <class U>
  Ref(Ref<U> &&other) noexcept : m_ptr(&static_cast<T &>(other.leakRef())) {
    TX_STATIC_ASSERT_IS_BASE_OF(T, U);
    TX_ASSERT(m_ptr);
  }

  Ref &operator=(Ref const &other) {
    if (this == &other) return *this;
    Ref copy = other;
    swap(copy);
    return *this;
  }

  Ref &operator=(Ref &&other) noexcept {
    Ref move = std::move(other);
    swap(move);
    return *this;
  }

  template <class U>
  Ref &operator=(Ref<U> &other) {
    TX_STATIC_ASSERT_IS_BASE_OF(T, U);
    Ref copy = other;
    swap(copy);
    return *this;
  }

  template <class U>
  Ref &operator=(Ref<U> &&other) {
    TX_STATIC_ASSERT_IS_BASE_OF(T, U);
    Ref move = std::move(other);
    swap(move);
    return *this;
  }

  bool operator==(const Ref &other) const { return m_ptr == other.m_ptr; }
  bool operator<(const Ref &other) const { return m_ptr < other.m_ptr; }
  bool operator!() const { return !m_ptr; }
  T *operator->() { return m_ptr; }

  void swap(Ref &other) noexcept { std::swap(m_ptr, other.m_ptr); }
  T *ptr() const { return m_ptr; }
  T &get() const { return *m_ptr; }
  TX_NODISCARD T &leakRef() {
    TX_ASSERT(m_ptr);
    return *TX_EXCHANGE(m_ptr, nullptr);
  }

 private:
  T *m_ptr;
};

// Unlike TX::Ref<T>, T can be type of std::nullptr_t.
template <class T>
class RefPtr {
 public:
  RefPtr() : m_ptr(nullptr) {}
  RefPtr(T *t) : m_ptr(ref(t)) {}
  RefPtr(std::nullptr_t) : m_ptr(nullptr) {}
  RefPtr(const RefPtr &other) : m_ptr(ref(other.get())) {}
  RefPtr(RefPtr &&other) noexcept : m_ptr(other.leakRef()) {}
  ~RefPtr() { deref(TX_EXCHANGE(m_ptr, nullptr)); }

  template <class U>
  RefPtr(const RefPtr<U> &other) : m_ptr(ref(other.get())) {
    TX_STATIC_ASSERT_IS_BASE_OF(T, U);
  }

  template <class U>
  RefPtr(RefPtr<U> &&other) noexcept : m_ptr(other.leakRef()) {}

  RefPtr &operator=(const RefPtr &other) {
    if (this == &other) return *this;
    RefPtr copy = other;
    swap(copy);
    return *this;
  }

  RefPtr &operator=(RefPtr &&other) noexcept {
    RefPtr move = std::move(other);
    swap(move);
    return *this;
  }

  template <class U>
  RefPtr &operator=(const RefPtr<U> &other) {
    RefPtr copy = other;
    swap(copy);
    return *this;
  }

  template <class U>
  RefPtr &operator=(RefPtr<U> &&other) noexcept {
    RefPtr move = std::move(other);
    swap(move);
    return *this;
  }

  RefPtr &operator=(T *t) {
    RefPtr p(t);
    swap(p);
    return *this;
  }

  RefPtr &operator=(std::nullptr_t) {
    deref(TX_EXCHANGE(m_ptr, nullptr));
    return *this;
  }

  bool operator==(const RefPtr &other) const { return m_ptr == other.m_ptr; }
  bool operator==(std::nullptr_t) const { return m_ptr == nullptr; }
  bool operator<(const RefPtr &other) const { return m_ptr < other.m_ptr; }
  bool operator!() const { return !m_ptr; }

  T *operator->() {
    TX_ASSERT(m_ptr);
    return m_ptr;
  }
  T &operator*() {
    TX_ASSERT(m_ptr);
    return *m_ptr;
  }

  explicit operator bool() const { return m_ptr != nullptr; }

  void swap(RefPtr &other) noexcept { std::swap(m_ptr, other.m_ptr); }
  T *get() const { return m_ptr; }
  TX_NODISCARD T *leakRef() { return TX_EXCHANGE(m_ptr, nullptr); }

 private:
  T *m_ptr;
};

template <class T>
Ref<T> adoptRef(T &t) {
  T::adopted(&t);
  return Ref<T>(t);
}

template <class T>
void swap(Ref<T> &a, Ref<T> &b) noexcept {
  a.swap(b);
}

template <class T>
RefPtr<T> adoptRef(T *t) {
  if (t) T::adopted(t);
  return RefPtr<T>(t);
}

template <class T>
void swap(RefPtr<T> &a, RefPtr<T> &b) noexcept {
  a.swap(b);
}
}  // namespace TX

template <class T>
struct std::hash<TX::Ref<T>> {
  size_t operator()(const TX::Ref<T> &t) const noexcept {
    return hash<T *>()(t.ptr());
  }
};

template <class T>
struct std::hash<TX::RefPtr<T>> {
  size_t operator()(const TX::RefPtr<T> &t) const noexcept {
    return hash<T *>()(t.get());
  }
};
