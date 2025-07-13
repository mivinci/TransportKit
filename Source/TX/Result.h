#pragma once

#include <functional>

#include "TX/Assert.h"
#include "TX/Log.h"
#include "TX/Union.h"

namespace TX {

template <class T, class E>
class Result {
 public:
  Result() = default;
  Result(T &&t) { TX_FORCE_DISCARD(m_union.template emplace<T>(t)); }
  Result(E &&e) { TX_FORCE_DISCARD(m_union.template emplace<E>(e)); }

  TX_NODISCARD bool isErr() const { return m_union.index() == 1; }
  TX_NODISCARD bool isOk() const { return m_union.index() == 0; }

  TX_NODISCARD const T &unwrap() const {
    TX_CHECK(!isErr(), "Called unwrap on an error");
    return std::get<T>(m_union);
  }

  TX_NODISCARD const E &unwrapErr() const {
    TX_CHECK(isErr(), "Called unwrapErr on an OK");
    return std::get<E>(m_union);
  }

  TX_NODISCARD const T &unwrapOr(const T &t) const { return isErr() ? t : std::get<T>(m_union); }

  TX_NODISCARD const T &unwrapOr(const T &&t) const { return isErr() ? t : std::get<T>(m_union); }

  TX_NODISCARD T unwrapOr(std::function<T()> f) const {
    return isErr() ? std::move(f()) : std::get<T>(m_union);
  }

 private:
  Union<T, E> m_union;
};

#define TX_OK(t) t
#define TX_ERR(e) e

}  // namespace TX
