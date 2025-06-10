#pragma once

#include <utility>

#include "TX/Platform.h"
#include "TX/Log.h"

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

  TX_NODISCARD bool IsSome() const { return some_; }
  TX_NODISCARD bool IsNone() const { return !some_; }

  T Unwrap() const {
    if (!some_) {
      TX_FATAL("called unwrap on None");
      TX_UNREACHABLE();
    }
    return t_;
  }

  T UnwrapOr(const T &other) const { return some_ ? t_ : other; }
  T UnwrapOr(T &&other) const { return some_ ? t_ : std::move(other); }
  T UnwrapOr(T (*f)()) const { return some_ ? t_ : f(); }

  T Take() {
    if (!some_) return None;
    auto t = std::move(*this);
    *this = None;
    return t;
  }

 private:
  bool some_;
  union {
    T t_;
  };
};

template <typename T>
Option<T> Some(T &t) {
  return Option<T>(t);
}
}  // namespace TX
