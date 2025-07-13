#include <gtest/gtest.h>

#include <atomic>

#include "TX/Condvar.h"
#include "TX/Own.h"
#include "TX/Thread.h"
#include "TX/WaitGroup.h"

namespace TX {
TEST(ThreadTest, Join) {
  int n = 114514;
  {
    Thread t([&]() { n = 42; });
  }
  EXPECT_EQ(n, 42);
}

TEST(ThreadTest, Detach) {
  Mutex<int> n = 114514;
  WaitGroup wg;
  wg.Add(1);
  Thread([&] {
    *n.Lock() = 42;
    wg.Done();
  }).Detach();
  wg.Wait();
  EXPECT_EQ(*n.Lock(), 42);
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
