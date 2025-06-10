#include <gtest/gtest.h>

#include <atomic>

#include "TX/Condvar.h"
#include "TX/Own.h"
#include "TX/Thread.h"

namespace TX {
TEST(ThreadTest, Join) {
  int n = 114514;
  {
    Thread t([&]() { n = 42; });
  }
  EXPECT_EQ(n, 42);
}

TEST(ThreadTest, Detach) {
  Condvar cv;
  Mutex n(114514);
  Thread t([&]() {
    *(n.Lock()) = 42;
    cv.NotifyOne();
  });
  t.Detach();
  auto n_guard = n.Lock();
  const bool timeout = cv.Wait(n_guard, Duration::Second(1));
  EXPECT_EQ(*n_guard, timeout ? 114514 : 42);
  if (timeout) {
    // In case of timeout, we need to wait for the detached thread to finish.
    // Otherwise, Unlock will be called on a destroyed mutex if the thread is
    // still running.
    cv.Wait(n_guard);
  }
}

TEST(ThreadTest, Adder) {
  constexpr int m = 3, N = 100;
  std::atomic n = 0;
  {
    std::vector<Own<Thread>> threads;
    threads.reserve(m);
    for (int i = 0; i < m; i++) {
      threads.push_back(Thread::Spawn([&]() {
        for (int j = 0; j < N; j++) {
          n.fetch_add(1);
        }
      }));
    }
  }
  EXPECT_EQ(n.load(), N * m);
}
}  // namespace TX
