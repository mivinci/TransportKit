#pragma once

#include "KFC/Assert.h"
#include "KFC/Memory.h"
#include "KFC/Preclude.h"

#if __cplusplus >= 201703L
#include <variant>
#else
#include <new>
#include <type_traits>
#endif

KFC_NAMESPACE_BEG

#if __cplusplus >= 201703L
template <class... Types> using OneOf = std::variant<Types...>;
#define KFC_ONEOF_GET(T, o) std::get<T>(o)
#define KFC_ONEOF_SET(T, o, v) (o).template emplace<T>(v)
#else
namespace _ {
template <size_t index, class T, class... Types> struct TypeIndex_;
template <size_t index, class T, class First, class... Rest>
struct TypeIndex_<index, T, First, Rest...> {
  static constexpr size_t Value =
      std::is_same<T, First>::value ? index : TypeIndex_<index + 1, T, Rest...>::Value;
};
template <size_t index, class T> struct TypeIndex_<index, T> {
  static constexpr size_t Value = index;
};

template <class T, class... Types>
constexpr size_t TypeIndex = typename TypeIndex_<0, T, Types...>::Value;
} // namespace _

template <class... Types> class OneOf {
public:
  explicit OneOf() : m_index(sizeof...(Types)) {}
  OneOf(const OneOf &other) : m_index(other.m_index) { copy(other); }
  OneOf(OneOf &&other) noexcept : m_index(other.m_index) { move(std::move(other)); }
  ~OneOf() { dispose(); }

  OneOf &operator=(const OneOf &other) {
    dispose();
    m_index = other.m_index;
    copy(other);
    return *this;
  }

  OneOf &operator=(OneOf &&other) noexcept {
    dispose();
    m_index = other.m_index;
    move(std::move(other));
    return *this;
  }

  template <class T> bool is() const { return m_index == _::TypeIndex<T, Types...>; }

  template <class T> T &get() const {
    KFC_STATIC_ASSERT((_::TypeIndex<T, Types...> < sizeof...(Types)), "Type not found in OneOf");
    return *reinterpret_cast<T *>(&m_storage);
  }

  template <class T, class... Args> void emplace(Args &&...args) {
    KFC_STATIC_ASSERT((_::TypeIndex<T, Types...> < sizeof...(Types)), "Type not found in OneOf");
    ctor(*reinterpret_cast<T *>(m_storage), std::forward<Args>(args)...);
    m_index = _::TypeIndex<T, Types...>;
  }

private:
  template <class T> bool tryDispose() {
    if (is<T>()) {
      dtor(*reinterpret_cast<T *>(&m_storage));
      m_index = sizeof...(Types);
    }
    return false;
  }

  template <class T> bool tryCopy(const OneOf &other) {
    if (other.is<T>()) {
      ctor(*reinterpret_cast<T *>(m_storage), other.get<T>());
    }
    return false;
  }

  template <class T> bool tryMove(OneOf &&other) {
    if (other.is<T>()) {
      ctor(*reinterpret_cast<T *>(m_storage), std::move(other.get<T>()));
    }
    return false;
  }

  void dispose() {
    if (m_index < sizeof...(Types)) {
      // Suppose that `Types` is `int, bool, char`, then
      // doAll(tryDispose<Types>...) expands to
      // doAll(
      //   tryDispose<int>,
      //   tryDispose<bool>,
      //   tryDispose<char>
      // )
      doAll(tryDispose<Types>...);
    }
  }

  void copy(const OneOf &other) { doAll(tryCopy<Types>(other)...); }
  void move(OneOf &&other) { doAll(tryMove<Types>(other)...); }

  using StorageType = typename std::aligned_union<0, Types...>::type;
  StorageType m_storage;
  size_t m_index;
};
#define KFC_ONEOF_GET(T, o) (o.get<T>())
#define KFC_ONEOF_SET(T, o, v) (o.emplace<T>(v))
#endif // __cplusplus >= 201703L

KFC_NAMESPACE_END
