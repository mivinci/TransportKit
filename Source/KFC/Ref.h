#pragma once

#include "KFC/Assert.h"
#include "KFC/Exchange.h"
#include "KFC/RefCounted.h"

namespace KFC {
// Guarantees that T is non-null unless moved and does automatic reference count
// management. Unlike std::shared_ptr<T>, KFC::Ref<T> is exactly the size of T*
// that is 8-byte on 64-bit systems and 4-byte on 32-bit systems. T must be a
// subclass of RefCounted.
template <class T> class Ref final {
public:
  explicit Ref(T &ptr) : m_ptr(ref(&ptr)) {}
  Ref(const Ref &other) : m_ptr(ref(other.ptr())) {}
  Ref(Ref &&other) noexcept : m_ptr(&other.leakRef()) { KFC_ASSERT(m_ptr); }
  ~Ref() { deref(KFC_EXCHANGE(m_ptr, nullptr)); }

  template <class U> Ref(const Ref<U> &other) : m_ptr(ref(other.ptr())) {}

  template <class U> Ref(Ref<U> &&other) noexcept : m_ptr(&static_cast<T &>(other.leakRef())) {
    KFC_STATIC_ASSERT_IS_BASE_OF(T, U);
    KFC_ASSERT(m_ptr);
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

  template <class U> Ref &operator=(Ref<U> &other) {
    KFC_STATIC_ASSERT_IS_BASE_OF(T, U);
    Ref copy = other;
    swap(copy);
    return *this;
  }

  template <class U> Ref &operator=(Ref<U> &&other) {
    KFC_STATIC_ASSERT_IS_BASE_OF(T, U);
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
  T &operator*() const { return *m_ptr; }
  KFC_NODISCARD T &leakRef() {
    KFC_ASSERT(m_ptr);
    return *KFC_EXCHANGE(m_ptr, nullptr);
  }

private:
  T *m_ptr;
};

// Unlike KFC::Ref<T>, T can be type of std::nullptr_t.
template <class T> class RefPtr {
public:
  RefPtr() : m_ptr(nullptr) {}
  RefPtr(T *t) : m_ptr(ref(t)) {}
  RefPtr(std::nullptr_t) : m_ptr(nullptr) {}
  RefPtr(const RefPtr &other) : m_ptr(ref(other.get())) {}
  RefPtr(RefPtr &&other) noexcept : m_ptr(other.leakRef()) {}
  ~RefPtr() { deref(KFC_EXCHANGE(m_ptr, nullptr)); }

  template <class U> RefPtr(const RefPtr<U> &other) : m_ptr(ref(other.get())) {
    KFC_STATIC_ASSERT_IS_BASE_OF(T, U);
  }

  template <class U> RefPtr(RefPtr<U> &&other) noexcept : m_ptr(other.leakRef()) {}

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

  template <class U> RefPtr &operator=(const RefPtr<U> &other) {
    RefPtr copy = other;
    swap(copy);
    return *this;
  }

  template <class U> RefPtr &operator=(RefPtr<U> &&other) noexcept {
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
    deref(KFC_EXCHANGE(m_ptr, nullptr));
    return *this;
  }

  bool operator==(const RefPtr &other) const { return m_ptr == other.m_ptr; }
  bool operator==(std::nullptr_t) const { return m_ptr == nullptr; }
  bool operator<(const RefPtr &other) const { return m_ptr < other.m_ptr; }
  bool operator!() const { return !m_ptr; }

  T *operator->() {
    KFC_ASSERT(m_ptr);
    return m_ptr;
  }
  T &operator*() {
    KFC_ASSERT(m_ptr);
    return *m_ptr;
  }

  explicit operator bool() const { return m_ptr != nullptr; }

  void swap(RefPtr &other) noexcept { std::swap(m_ptr, other.m_ptr); }
  T *get() const { return m_ptr; }
  KFC_NODISCARD T *leakRef() { return KFC_EXCHANGE(m_ptr, nullptr); }

private:
  T *m_ptr;
};

template <class T> Ref<T> adoptRef(T &t) {
  T::adopted(&t);
  return Ref<T>(t);
}

template <class T> void swap(Ref<T> &a, Ref<T> &b) noexcept { a.swap(b); }

template <class T> RefPtr<T> adoptRef(T *t) {
  if (t) T::adopted(t);
  return RefPtr<T>(t);
}

template <class T> void swap(RefPtr<T> &a, RefPtr<T> &b) noexcept { a.swap(b); }
} // namespace KFC

template <class T> struct std::hash<KFC::Ref<T>> {
  size_t operator()(const KFC::Ref<T> &t) const noexcept { return hash<T *>()(t.ptr()); }
};

template <class T> struct std::hash<KFC::RefPtr<T>> {
  size_t operator()(const KFC::RefPtr<T> &t) const noexcept { return hash<T *>()(t.get()); }
};
