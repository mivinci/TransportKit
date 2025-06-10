#include "TX/runtime/Async.h"
#include "gtest/gtest.h"

namespace TX {
class MockScheduler {
public:
  struct Sync {
    struct promise_type {
      Sync get_return_object() noexcept { return Sync{}; }
      std::suspend_never initial_suspend() noexcept { return {}; }
      std::suspend_never final_suspend() noexcept { return {}; }
      void return_void() {}
      void unhandled_exception() {}
    };
  };

  template <class P> Sync launch(P &&p) { co_await p; }
  template <class F> void block(F &&f) { launch(std::forward<F>(f)()); }
};

Async<int> foo() { co_return 1; }
Async<int> sum() {
  int m = 0;
  for (int i = 0; i < 114514; i++) {
    m += co_await foo();
  }
  co_return m;
}

TEST(AsyncTest, Simple) {
  MockScheduler sched;
  sched.block([]() -> Async<int> {
    auto m = co_await sum();
    EXPECT_EQ(m, 114514);
    co_return m;
  });
}
} // namespace TX
