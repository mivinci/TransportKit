#pragma once

#include "KFC/Platform.h"
#if __cplusplus >= 201703L
#include <optional>
#else
#include <utility>

#include "KFC/Log.h"
#endif

namespace KFC {
#if __cplusplus >= 201703L
template <class T>
using Option = std::optional<T>;
static std::nullopt_t None = std::nullopt;
#else
class None {};
static constexpr None None;

template <class T>
class Option {
 public:
  Option() : some_(false) {}
  Option(class None) : some_(false) {}
  Option(T &t) : some_(true), t_(t) {}
  Option(T &&t) : some_(true), t_(std::move(t)) {}
  Option(Option &other) : some_(other.some_), t_(other.t_) {}
  Option(Option &&other) noexcept : some_(other.some_), t_(other.t_) { other = None; }
  // value's destructor can be non-trivial, so we have to call it explicitly.
  ~Option() { t_.~T(); }

  Option &operator=(class None) {
    if (some_) {
      t_.~T();
      some_ = false;
    }
    return *this;
  }

  Option &operator=(const Option &other) {
    some_ = other.some_;
    t_ = other.t_;
    return *this;
  };
  Option &operator=(Option &&other) noexcept {
    some_ = other.some_;
    t_ = other.t_;
    other = None;
    return *this;
  }

  bool operator==(class None) const { return !some_; }
  bool operator==(const Option &other) const { return some_ ? t_ == other.t_ : other == None; }

  explicit operator bool() const { return isSome(); }

  KFC_NODISCARD bool isSome() const { return some_; }
  KFC_NODISCARD bool isNone() const { return !some_; }

  T &unwrap() {
    if (!some_) {
      KFC_THROW("called unwrap on None");
      KFC_UNREACHABLE();
    }
    return t_;
  }
  T &unwrapOr(const T &other) { return some_ ? t_ : other; }
  T &unwrapOr(T &&other) { return some_ ? t_ : std::move(other); }
  T &unwrapOr(T (*f)()) { return some_ ? t_ : f(); }

  T take() {
    if (!some_) return None;
    return KFC_EXCHANGE(*this, None);
  }

 private:
  bool some_;
  union {
    T t_;
  };
};
#endif

template <typename T>
constexpr Option<T> Some(T &t) {
  return t;
}

#if __cplusplus >= 201703L
#define KFC_ISSOME(x) ((x).has_value())
#define KFC_UNWRAP(x) ((x).value())
#define KFC_UNWRAP_OR(x, y) ((x).value_or(y))
#else
#define KFC_ISSOME(x) ((x).isSome())
#define KFC_UNWRAP(x) ((x).unwrap())
#define KFC_UNWRAP_OR(x, y) ((x).unwrapOr(y))
#endif

#if __cplusplus >= 201703L
#define KFC_IF_SOME(name, expr)                                    \
  KFC_SILENCE_DANGLING_ELSE_BEGIN                                  \
  if (auto KFC_UNIQUE_NAME(_##name) = expr)                        \
    if (auto &name = KFC_UNWRAP(KFC_UNIQUE_NAME(_##name)); false) { \
    } else                                                        \
      KFC_SILENCE_DANGLING_ELSE_END

#define KFC_IF_SOME_CONST(name, expr)                                    \
  KFC_SILENCE_DANGLING_ELSE_BEGIN                                        \
  if (auto KFC_UNIQUE_NAME(_##name) = expr)                              \
    if (const auto &name = KFC_UNWRAP(KFC_UNIQUE_NAME(_##name)); false) { \
    } else                                                              \
      KFC_SILENCE_DANGLING_ELSE_END
#else
thread_local static bool tx_if_some_magic_variable = false;
// If you have to use KFC_IF_SOME, consider use C++17 instead.
#define KFC_IF_SOME(name, expr)                                                           \
  KFC::tx_if_some_magic_variable = true;                                                  \
  if (auto KFC_UNIQUE_NAME(_##name) = expr)                                               \
    for (auto &name = KFC_UNWRAP(KFC_UNIQUE_NAME(_##name)); KFC::tx_if_some_magic_variable; \
         KFC::tx_if_some_magic_variable = false)
#define KFC_IF_SOME_CONST KFC_IF_SOME
#endif
}  // namespace KFC
