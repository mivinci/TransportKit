#pragma once

#include <utility>

#include "TX/Log.h"
#include "TX/Platform.h"

namespace TX {

class None {};
static constexpr None None;

template <typename T>
class Option {
 public:
  Option() : some_(false) {}
  Option(class None) : some_(false) {}
  Option(T &t) : some_(true), t_(t) {}
  Option(T &&t) : some_(true), t_(std::move(t)) {}
  Option(Option &other) : some_(other.some_), t_(other.t_) {}
  Option(Option &&other) noexcept : some_(other.some_), t_(other.t_) {
    other = None;
  }
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
  bool operator==(const Option &other) const {
    return some_ ? t_ == other.t_ : other == None;
  }

  explicit operator bool() const { return IsSome(); }

  TX_NODISCARD bool IsSome() const { return some_; }
  TX_NODISCARD bool IsNone() const { return !some_; }

  T &Unwrap() {
    if (!some_) {
      TX_FATAL("called unwrap on None");
      TX_UNREACHABLE();
    }
    return t_;
  }
  T &UnwrapOr(const T &other) { return some_ ? t_ : other; }
  T &UnwrapOr(T &&other) { return some_ ? t_ : std::move(other); }
  T &UnwrapOr(T (*f)()) { return some_ ? t_ : f(); }

  T Take() {
    if (!some_) return None;
    // auto t = std::move(*this);
    // *this = None;
    return std::exchange(*this, None);
  }

 private:
  bool some_;
  union {
    T t_;
  };
};

template <typename T>
constexpr Option<T> Some(T &t) {
  return t;
}

#define TX_IF_SOME(name, expr)                                  \
  TX_SILENCE_DANGLING_ELSE_BEGIN                                \
  if (auto TX_UNIQUE_NAME(_##name) = expr)                      \
    if (auto &name = TX_UNIQUE_NAME(_##name).Unwrap(); false) { \
    } else                                                      \
      TX_SILENCE_DANGLING_ELSE_END

#define TX_IF_SOME_CONST(name, expr)                                  \
  TX_SILENCE_DANGLING_ELSE_BEGIN                                      \
  if (auto TX_UNIQUE_NAME(_##name) = expr)                            \
    if (const auto &name = TX_UNIQUE_NAME(_##name).Unwrap(); false) { \
    } else                                                            \
      TX_SILENCE_DANGLING_ELSE_END

}  // namespace TX
