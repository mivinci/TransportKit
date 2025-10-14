#pragma once

#include <utility>

namespace KFC {
struct Void {};

namespace _ {
// clang-format off
template <class T> struct FixVoid { typedef T Type; };
template <> struct FixVoid<void> { typedef Void Type; };
template <class T> struct UnfixVoid { typedef T Type; };
template <> struct UnfixVoid<Void> { typedef void Type; };
// clang-format on

template <class Func, class... Args> struct ReturnType {
  typedef decltype(std::declval<Func>()(std::declval<Args>()...)) Type;
};
template <class Func> struct ReturnType<Func, void> {
  typedef decltype(std::declval<Func>()()) Type;
};
} // namespace _

template <class T> using FixVoid = typename _::FixVoid<T>::Type;
template <class T> using UnfixVoid = typename _::UnfixVoid<T>::Type;
template <class Func, class... Args> using ReturnType = typename _::ReturnType<Func, Args...>::Type;

template <class Out, class... In> struct FunctionCaller {
  template <class Func> static Out apply(Func &func, In &&...in) {
    return func(std::forward<In>(in)...);
  }
};

template <class Out> struct FunctionCaller<Out> {
  template <class Func> static Out apply(Func &func) { return func(); }
};

template <class... In> struct FunctionCaller<void, In...> {
  template <class Func> static Void apply(Func &func, In &&...in) {
    func(std::forward<In>(in)...);
    return Void{};
  }
};

template <> struct FunctionCaller<void> {
  template <class Func> static Void apply(Func &func) {
    func();
    return Void{};
  }
};

template <class Out> struct FunctionCaller<Out, Void> {
  template <class Func> static Out apply(Func &func, const Void &) { return func(); }
};

template <class... In> struct FunctionCaller<Void, In...> {
  template <class Func> static Void apply(Func &func, In &&...in) {
    func(std::forward<In>(in)...);
    return Void{};
  }
};

template <> struct FunctionCaller<Void, Void> {
  template <class Func> static Void apply(Func &func, const Void &) {
    func();
    return Void{};
  }
};

template <> struct FunctionCaller<Void> {
  template <class Func> static Void apply(Func &func) {
    func();
    return Void{};
  }
};

} // namespace KFC
