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
  explicit RefCounted() : ref_count_(0), adopted_(false) {}
  virtual ~RefCounted() = default;
  void ref() { ++ref_count_; }
  void deref() {
    if (!--ref_count_) delete const_cast<T *>(static_cast<const T *>(this));
  }
  TX_NODISCARD unsigned refCount() const { return ref_count_; }
  TX_DISALLOW_COPY(RefCounted)

  static void adopted(RefCounted *p) {
    if (p->adopted_) return;
    p->adopted_ = true;
  }

 private:
  Type ref_count_;
  bool adopted_;
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
  explicit Ref(T &t) : t_(ref(&t)) {}
  Ref(const Ref &other) : t_(ref(other.ptr())) {}
  Ref(Ref &&other) noexcept : t_(&other.leakRef()) { TX_ASSERT(t_); }
  ~Ref() { deref(TX_EXCHANGE(t_, nullptr)); }

  template <class U>
  Ref(const Ref<U> &other)
      : t_(ref(other.ptr())) {} /* NOLINT(*-explicit-constructor) */

  template <class U>
  Ref(Ref<U> &&other) noexcept
      : t_(&static_cast<T &>(
            other.leakRef())) { /* NOLINT(*-explicit-constructor) */
    TX_STATIC_ASSERT_IS_BASE_OF(T, U);
    TX_ASSERT(t_);
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

  bool operator==(const Ref &other) const { return t_ == other.t_; }
  bool operator<(const Ref &other) const { return t_ < other.t_; }
  bool operator!() const { return !t_; }
  T *operator->() { return t_; }

  void swap(Ref &other) noexcept { std::swap(t_, other.t_); }
  T *ptr() const { return t_; }
  T &get() const { return *t_; }
  TX_NODISCARD T &leakRef() {
    TX_ASSERT(t_);
    return *TX_EXCHANGE(t_, nullptr);
  }

 private:
  T *t_;
};

// Unlike TX::Ref<T>, T can be type of std::nullptr_t.
template <class T>
class RefPtr {
 public:
  RefPtr() : t_(nullptr) {}
  RefPtr(T *t) : t_(ref(t)) {}            /* NOLINT(*-explicit-constructor) */
  RefPtr(std::nullptr_t) : t_(nullptr) {} /* NOLINT(*-explicit-constructor) */
  RefPtr(const RefPtr &other) : t_(ref(other.get())) {}
  RefPtr(RefPtr &&other) noexcept : t_(other.leakRef()) {}
  ~RefPtr() { deref(TX_EXCHANGE(t_, nullptr)); }

  template <class U>
  RefPtr(const RefPtr<U> &other)
      : t_(ref(other.get())) { /* NOLINT(*-explicit-constructor) */
    TX_STATIC_ASSERT_IS_BASE_OF(T, U);
  }

  template <class U>
  RefPtr(RefPtr<U> &&other) noexcept
      : t_(other.leakRef()) {} /* NOLINT(*-explicit-constructor) */

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
    deref(TX_EXCHANGE(t_, nullptr));
    return *this;
  }

  bool operator==(const RefPtr &other) const { return t_ == other.t_; }
  bool operator==(std::nullptr_t) const { return t_ == nullptr; }
  bool operator<(const RefPtr &other) const { return t_ < other.t_; }
  bool operator!() const { return !t_; }

  T *operator->() {
    TX_ASSERT(t_);
    return t_;
  }
  T &operator*() {
    TX_ASSERT(t_);
    return *t_;
  }

  explicit operator bool() const { return !!t_; }

  void swap(RefPtr &other) noexcept { std::swap(t_, other.t_); }
  T *get() const { return t_; }
  TX_NODISCARD T *leakRef() { return TX_EXCHANGE(t_, nullptr); }

 private:
  T *t_;
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
