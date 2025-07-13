#pragma once

#include <utility>

namespace TX {
template<class T, class U = T>
T exchange(T& obj, U&& new_val) {
  T old_val = std::move(obj);
  obj = std::forward<U>(new_val);
  return old_val;
}

#if __cplusplus >= 201402L
#define TX_EXCHANGE(a, b) (std::exchange(a, b))
#else
#define TX_EXCHANGE(a, b) (TX::exchange(a, b))
#endif

}
