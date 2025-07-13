#pragma once

#if __cplusplus >= 201703L
#include <variant>
#else
#include "KFC/Preclude.h"
#include <new>
#include <type_traits>
#endif

namespace KFC {

#if __cplusplus >= 201703L
template <class... Types> using OneOf = std::variant<Types...>;
constexpr std::size_t kOneOfNoPos = std::variant_npos;
#define KFC_ONEOF_GET(T, o) std::get<T>(o)
#define KFC_ONEOF_SET(T, o, v) (o).template emplace<T>(v)
#else
// TODO
#endif // __cplusplus >= 201703L
} // namespace KFC
