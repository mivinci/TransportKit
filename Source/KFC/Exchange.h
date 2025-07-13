#pragma once

#include <utility>

namespace KFC {
template<class T, class U = T>
T exchange(T& obj, U&& new_val) {
  T old_val = std::move(obj);
  obj = std::forward<U>(new_val);
  return old_val;
}

#if __cplusplus >= 201402L
#define KFC_EXCHANGE(a, b) (std::exchange(a, b))
#else
#define KFC_EXCHANGE(a, b) (KFC::exchange(a, b))
#endif

}
