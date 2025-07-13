#pragma once

#include <functional>

#include "KFC/Assert.h"
#include "KFC/Log.h"
#include "KFC/OneOf.h"
#include "KFC/Option.h"

namespace KFC {

// A class that represents a result of an operation that can either succeed or fail.
template <class T, class E> class Result {
public:
  Result() = default;
  Result(const T &t) { KFC_ONEOF_SET(T, m_oneOf, t); }
  Result(const E &e) { KFC_ONEOF_SET(E, m_oneOf, e); }
  Result(T &&t) { KFC_ONEOF_SET(T, m_oneOf, std::move(t)); }
  Result(E &&e) { KFC_ONEOF_SET(E, m_oneOf, std::move(e)); }

  KFC_NODISCARD bool isErr() const { return m_oneOf.index() == 2; }
  KFC_NODISCARD bool isOk() const { return m_oneOf.index() == 1; }
  KFC_NODISCARD bool isEmpty() const { return m_oneOf.index() == 0; }

  Option<T> takeOk() const {
    return isErr() ? None : Option<T>(std::move(KFC_ONEOF_GET(T, m_oneOf)));
  }

  Option<E> takeErr() const {
    return isErr() ? Option<E>(std::move(KFC_ONEOF_GET(E, m_oneOf))) : None;
  }

  // Unwrap the value if it is OK, otherwise throw.
  T &unwrap() const {
    KFC_CHECK(!isErr(), "called unwrap on an error");
    return const_cast<T &>(KFC_ONEOF_GET(T, m_oneOf));
  }

  // Unwrap the error if it is an error, otherwise throw.
  E &unwrapErr() const {
    KFC_CHECK(isErr(), "called unwrapErr on an OK");
    return const_cast<E &>(KFC_ONEOF_GET(E, m_oneOf));
  }

  // Unwrap the value if it is OK, otherwise return the given value.
  const T &unwrapOr(const T &t) const { return isErr() ? t : KFC_ONEOF_GET(T, m_oneOf); }

  // A function that takes a function and applies it to the value if it is OK. The function should
  // be callable with T and return a value of type U. If the value is an error, it returns the error
  // wrapped in a Result<U, E>.
  template <class U, class Func> Result<U, E> map(Func &&func) const {
    return isErr() ? Result<U, E>(KFC_ONEOF_GET(E, m_oneOf))
                   : Result<U, E>(func(KFC_ONEOF_GET(T, m_oneOf)));
  }

private:
  OneOf<std::monostate, T, E> m_oneOf;
};

// A specialization of Result for void.
template <class E> class Result<void, E> {
public:
  Result() : m_err(None) {}
  Result(const E &e) : m_err(e) {}
  Result(E &&e) : m_err(std::move(e)) {}

  KFC_NODISCARD bool isErr() const { return m_err.isSome(); }
  KFC_NODISCARD bool isOk() const { return m_err.isNone(); }

  Option<E> takeErr() const { return std::move(m_err); }
  void unwrap() const { KFC_CHECK(!isErr(), "called unwrap on an error"); }
  E &unwrapErr() const { return m_err.unwrap(); }

private:
  Option<E> m_err;
};

#define KFC_OK(t) t
#define KFC_ERR(e) e

} // namespace KFC
