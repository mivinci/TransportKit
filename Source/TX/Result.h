#pragma once
#include <variant>

#include "TX/Option.h"
#include "TX/Platform.h"
#include "TX/String.h"
#include "TX/Log.h"

namespace TX {
template <class T, class E>
class Result {
 public:
  Result() {}
  Result(T t) : inner_(t) {}
  Result(E e) : inner_(e) {}
  Result(const Result &other) : inner_(other.inner_) {}
  Result(Result &&other) noexcept : inner_(std::move(other.inner_)) {
    // No need to reset other.inner_ for std::variant
  }
  Result &operator=(const Result &other) = default;
  Result &operator=(Result &&other) noexcept {
    inner_ = std::move(other.inner_);
    // No need to reset other.inner_ for std::variant
    return *this;
  }
  Result &operator=(T &&t) noexcept {
    inner_.template emplace<T>(t);
    return *this;
  }
  Result &operator=(E &&e) noexcept {
    inner_.template emplace<E>(e);
    return *this;
  }

  bool IsOk() { return std::holds_alternative<T>(inner_); }
  bool IsErr() { return std::holds_alternative<E>(inner_); }

  T Unwrap() {
    if (IsErr()) TX_FATAL("called unwrap on an error");
    return std::get<T>(inner_);
  }

  T UnwrapOr(T other) { return IsErr() ? other : std::get<T>(inner_); }
  T UnwrapOr(T (*f)()) { return IsErr() ? f() : std::get<T>(inner_); }

  E UnwrapErr() {
    if (IsOk()) TX_FATAL("called unwrap on an ok");
    return std::get<E>(inner_);
  }

  T Expect(const String &hint) {
    if (IsErr()) TX_FATAL("called unwrap on an error: {:s}", hint);
    return std::get<T>(inner_);
  }

  template <class V, class F>
  Result<V, E> Map(F f) {
    return IsErr() ? Result<V, E>::Err(std::get<E>(inner_))
                   : Result<V, E>::Ok(f());
  }

  static Result Ok(T t) { return Result(t); }
  static Result Err(E e) { return Result(e); }

 private:
  std::variant<T, E> inner_;
};

template <class E>
class Result<void, E> {
 public:
  Result() : err(None) {}
  Result(E e) : err(e) {}
  Result(const Result &other) : err(other.err) {}
  Result(Result &&other) : err(std::move(other.err)) { other.err = None; }
  Result &operator=(const Result &other) {
    err = other.err;
    return *this;
  }
  Result &operator=(Result &&other) {
    err = std::move(other.err);
    other.err = None;
    return *this;
  }
  Result &operator=(E &&e) noexcept {
    err = e;
    return *this;
  }

  bool IsOk() { return err.IsNone(); }
  bool IsErr() { return err.IsSome(); }

  void Unwrap() {
    if (IsErr()) TX_FATAL("called unwrap on an error");
  }
  void UnwrapOr() {}
  void UnwrapOr(void (*f)()) {
    if (IsErr()) f();
  }
  E UnwrapErr() {
    if (IsOk()) TX_FATAL("called unwrap on an ok");
    return err.Unwrap();
  }

  void Expect(const String &hint) {
    if (IsErr()) TX_FATAL("called unwrap on an error: {:s}", hint);
  }

  template <class V, class F>
  Result<V, E> Map(F f) {
    return IsErr() ? Result<V, E>::Err(err.Unwrap()) : Result<V, E>::Ok(f());
  }

  static Result Ok() { return Result(); }
  static Result Err(E e) { return Result(e); }

 private:
  Option<E> err;
};
}  // namespace TX
