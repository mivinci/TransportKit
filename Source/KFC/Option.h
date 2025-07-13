#pragma once
#include <utility>

#include "KFC/Assert.h"
#include "KFC/Memory.h"
#include "KFC/Preclude.h"

KFC_NAMESPACE_BEG

class None {};
static constexpr None None;

template <class T> class Option {
public:
  constexpr Option() : m_hasValue(false) {}
  constexpr Option(class None) : m_hasValue(false) {}
  constexpr Option(const T &t) : m_hasValue(true), m_value(t) {}
  constexpr Option(T &t) : m_hasValue(true), m_value(t) {}
  constexpr Option(T &&t) : m_hasValue(true), m_value(std::move(t)) {}
  constexpr Option(const Option &other) : m_hasValue(other.m_hasValue), m_value(other.m_value) {}
  constexpr Option(Option &other) : m_hasValue(other.m_hasValue), m_value(other.m_value) {}
  Option(Option &&other) noexcept
      : m_hasValue(other.m_hasValue), m_value(std::move(other.m_value)) {
    other = None;
  }
  ~Option() {
    if (m_hasValue) dtor(m_value);
  }

  Option &operator=(class None) {
    if (m_hasValue) {
      dtor(m_value);
      m_hasValue = false;
    }
    return *this;
  }

  Option &operator=(const Option &other) {
    if (this != &other) {
      if (m_hasValue) {
        dtor(m_value);
        m_hasValue = false;
      }
      if (other.m_hasValue) {
        ctor(m_value, other.m_value);
        m_hasValue = true;
      }
    }
    return *this;
  }

  Option &operator=(Option &&other) noexcept {
    if (this != &other) {
      if (m_hasValue) {
        dtor(m_value);
        m_hasValue = false;
      }
      if (other.m_hasValue) {
        ctor(m_value, std::move(other.m_value));
        m_hasValue = true;
      }
    }
    return *this;
  }

  bool operator==(class None) const { return !m_hasValue; }
  bool operator==(const Option &other) const {
    return m_hasValue ? m_value == other.m_value : other == None;
  }

  explicit operator bool() const { return isSome(); }

  KFC_NODISCARD bool isSome() const { return m_hasValue; }
  KFC_NODISCARD bool isNone() const { return !m_hasValue; }

  T &unwrap() {
    KFC_CHECK(m_hasValue, "called unwrap on None");
    return m_value;
  }

  T &unwrapOr(const T &other) { return m_hasValue ? m_value : other; }

  Option<T> take() {
    if (!m_hasValue) return None;
    return KFC_EXCHANGE(*this, None);
  }

  template <class... Args> T &emplace(Args &&...args) {
    if (m_hasValue) m_value.~T();
    m_hasValue = true;
    new (&m_value) T(std::forward<Args>(args)...);
    return m_value;
  }

private:
  bool m_hasValue;
  union {
    T m_value;
  };
};

template <class T> class Option<T &> {
public:
  constexpr Option() : m_ptr(nullptr) {}
  constexpr Option(class None) : m_ptr(nullptr) {}
  constexpr Option(T &t) : m_ptr(&t) {}
  constexpr Option(T *t) : m_ptr(t) {}
  constexpr Option(const Option &other) : m_ptr(other.m_ptr) {}
  constexpr Option(Option &other) : m_ptr(other.m_ptr) {}
  Option(Option &&other) noexcept : m_ptr(other.m_ptr) { other = None; }
  ~Option() = default;

  Option &operator=(class None) {
    m_ptr = nullptr;
    return *this;
  }

  Option &operator=(const Option &other) = default;

  Option &operator=(Option &&other) noexcept {
    m_ptr = other.m_ptr;
    other = None;
    return *this;
  }

  Option &operator=(T &t) {
    m_ptr = &t;
    return *this;
  }

  bool operator==(class None) const { return m_ptr == nullptr; }
  bool operator==(const Option &other) const { return m_ptr == other.m_ptr; }
  explicit constexpr operator bool() const { return isSome(); }

  KFC_NODISCARD constexpr bool isSome() const { return m_ptr != nullptr; }
  KFC_NODISCARD constexpr bool isNone() const { return m_ptr == nullptr; }

  T &unwrap() {
    KFC_CHECK(m_ptr, "called unwrap on None");
    return *m_ptr;
  }

  T &unwrapOr(const T &other) { return m_ptr ? *m_ptr : other; }

  Option<T> take() {
    if (!m_ptr) return None;
    return KFC_EXCHANGE(*this, None);
  }

private:
  T *m_ptr;
};

template <typename T> constexpr Option<T> Some(const T &t) { return t; }

#define KFC_ISSOME(x) ((x).isSome())
#define KFC_UNWRAP(x) ((x).unwrap())
#define KFC_UNWRAP_OR(x, y) ((x).unwrapOr(y))

#if __cplusplus >= 201703L
#define KFC_IF_SOME(name, expr)                                                                    \
  KFC_SILENCE_DANGLING_ELSE_BEGIN                                                                  \
  if (auto &&KFC_UNIQUE_NAME(_##name) = expr)                                                      \
    if (auto &name = KFC_UNWRAP(KFC_UNIQUE_NAME(_##name)); false) {                                \
    } else                                                                                         \
      KFC_SILENCE_DANGLING_ELSE_END

#define KFC_IF_SOME_CONST(name, expr)                                                              \
  KFC_SILENCE_DANGLING_ELSE_BEGIN                                                                  \
  if (auto &&KFC_UNIQUE_NAME(_##name) = expr)                                                      \
    if (const auto &name = KFC_UNWRAP(KFC_UNIQUE_NAME(_##name)); false) {                          \
    } else                                                                                         \
      KFC_SILENCE_DANGLING_ELSE_END
#else
#endif

KFC_NAMESPACE_END
