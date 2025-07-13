#pragma once
#include <new>
#include <type_traits>
#if __cplusplus >= 201703L
#include <variant>
#endif

#include "KFC/Platform.h"

namespace KFC {

#if __cplusplus >= 201703L
template <class... Types>
using Union = std::variant<Types...>;
constexpr std::size_t UnionNone = std::variant_npos;
#else
constexpr std::size_t UnionNone = -1;

template <class... Types>
class Union {
 public:
  explicit Union() : m_index(UnionNone) {}
  ~Union() {
    if (m_index == UnionNone) return;
    // TODO:
    m_index = UnionNone;
  }

  template <class T>
  KFC_CONSTEXPR_FUNCTION T &get() const noexcept {
    return *const_cast<T *>(reinterpret_cast<const T *>(&m_storage));
  }

  template <class T, class... Args>
  T &emplace(Args &&...args) noexcept {
    new (&m_storage) T(std::forward<Args>(args)...);
    m_index = TypeIndex<T, Types...>::value;
    return get<T>();
  }

  KFC_CONSTEXPR_FUNCTION std::size_t index() const { return m_index; }

  template <class T>
  KFC_CONSTEXPR_FUNCTION bool is() const noexcept {
    return m_index == TypeIndex<T, Types...>::value;
  }

 private:
  template <class T, class... Rest>
  struct TypeIndex {
    static constexpr int value = 0;
  };

  template <class T, class First, class... Rest>
  struct TypeIndex<T, First, Rest...> {
    static constexpr int value =
        std::is_same<T, First>::value ? 0 : 1 + TypeIndex<T, Rest...>::value;
  };

  using StorageType = typename std::aligned_union<0, Types...>::type;

  std::size_t m_index;
  StorageType m_storage;
};
#endif  // __cplusplus >= 201703L
}  // namespace KFC
