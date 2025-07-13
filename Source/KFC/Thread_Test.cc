#include <atomic>

#include "KFC/Condition.h"
#include "KFC/Own.h"
#include "KFC/Testing.h"
#include "KFC/Thread.h"
#include "KFC/WaitGroup.h"

namespace KFC {
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
  wg.add(1);
  Thread([&] {
    *n.lock() = 42;
    wg.done();
  }).detach();
  wg.wait();
  EXPECT_EQ(*n.lock(), 42);
}

TEST(ThreadTest, Adder) {
  constexpr int m = 3, N = 100;
  std::atomic n = 0;
  {
    std::vector<OwnThread> threads;
    threads.reserve(m);
    for (int i = 0; i < m; i++) {
      threads.push_back(Thread::spawn([&]() {
        for (int j = 0; j < N; j++) {
          n.fetch_add(1);
        }
      }));
    }
  }
  EXPECT_EQ(n.load(), N * m);
}

TEST(ThreadTest, Exception) {
  EXPECT_THROW(Thread::spawn([] { throw std::runtime_error("test"); }), Exception);
}
} // namespace KFC
