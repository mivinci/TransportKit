#pragma once

#include <functional>

#include "KFC/Assert.h"
#include "KFC/Log.h"
#include "KFC/Union.h"

namespace KFC {

template <class T, class E>
class Result {
 public:
  Result() = default;
  Result(T &&t) { KFC_FORCE_DISCARD(m_union.template emplace<T>(t)); }
  Result(E &&e) { KFC_FORCE_DISCARD(m_union.template emplace<E>(e)); }

  KFC_NODISCARD bool isErr() const { return m_union.index() == 1; }
  KFC_NODISCARD bool isOk() const { return m_union.index() == 0; }

  KFC_NODISCARD const T &unwrap() const {
    KFC_CHECK(!isErr(), "Called unwrap on an error");
    return std::get<T>(m_union);
  }

  KFC_NODISCARD const E &unwrapErr() const {
    KFC_CHECK(isErr(), "Called unwrapErr on an OK");
    return std::get<E>(m_union);
  }

  KFC_NODISCARD const T &unwrapOr(const T &t) const { return isErr() ? t : std::get<T>(m_union); }

  KFC_NODISCARD const T &unwrapOr(const T &&t) const { return isErr() ? t : std::get<T>(m_union); }

  KFC_NODISCARD T unwrapOr(std::function<T()> f) const {
    return isErr() ? std::move(f()) : std::get<T>(m_union);
  }

 private:
  Union<T, E> m_union;
};

#define KFC_OK(t) t
#define KFC_ERR(e) e

}  // namespace KFC
