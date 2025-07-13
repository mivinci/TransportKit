#pragma once
#include "TX/Result.h"
#include <algorithm>
#include <coroutine>
#include <exception>

namespace TX {
template <class T> class Promise;
template <class T> class [[nodiscard]] Async {
public:
  using promise_type = Promise<T>;
  using Handle = std::coroutine_handle<promise_type>;
  using Output = T;

  ~Async() {
    if (handle_)
      handle_.destroy();
  }

  bool await_ready() { return false; }
  T await_resume() { return handle_.promise().value_; }
  template <class PromiseType>
  Handle await_suspend(std::coroutine_handle<PromiseType> h) {
    handle_.promise().continuation_ = h;
    return handle_;
  }

private:
  friend Promise<T>;
  explicit Async(Handle h) : handle_(h) {}
  Handle handle_;
};

template <class T> class Promise {
public:
  Async<T> get_return_object() noexcept {
    return Async<T>(Async<T>::Handle::from_promise(*this));
  }

  struct InitialAwaiter {
    bool await_ready() noexcept { return false; }
    void await_resume() noexcept {}
    void await_suspend(std::coroutine_handle<> h) noexcept {}
  };

  struct FinalAwaiter {
    bool await_ready() noexcept { return false; }
    void await_resume() noexcept {}
    template <class PromiseType>
    auto await_suspend(std::coroutine_handle<PromiseType> h) noexcept {
      return h.promise().continuation_;
    }
  };

  InitialAwaiter initial_suspend() noexcept { return {}; }
  FinalAwaiter final_suspend() noexcept { return {}; }
  void return_value(T t) { value_ = std::move(t); }
  void unhandled_exception() { eptr_ = std::current_exception(); }

private:
  friend Async<T>;
  std::coroutine_handle<> continuation_;
  std::exception_ptr eptr_;
  T value_;
};
} // namespace TX
