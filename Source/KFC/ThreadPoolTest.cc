#include "KFC/Test.h"
#include "ThreadPool.h"

namespace KFC {
TEST(BlockingPoolTest, Simple1) {
  ThreadPool pool(1);
  pool.spawn([] {});
  pool.spawn([] {});
}

TEST(BlockingPoolTest, Simple2) {
  ThreadPool pool(2);
  pool.spawn([] {});
  pool.spawn([] {});
}

TEST(BlockingPoolTest, Adder1) {
  constexpr int N = 42;
  std::atomic n = 0;
  {
    ThreadPool pool(1);
    for (int i = 0; i < 42; i++) {
      pool.spawn([&] { n.fetch_add(1); });
    }
  }
  EXPECT_EQ(n.load(), N);
}

TEST(BlockingPoolTest, Adder2) {
  constexpr int N = 42;
  std::atomic n = 0;
  {
    ThreadPool pool(2);
    for (int i = 0; i < 42; i++) {
      pool.spawn([&] { n.fetch_add(1); });
    }
  }
  EXPECT_EQ(n.load(), N);
}

TEST(BlockingPoolTest, Adder3) {
  constexpr int N = 42;
  std::atomic n = 0;
  {
    ThreadPool pool(3);
    for (int i = 0; i < 42; i++) {
      pool.spawn([&] { n.fetch_add(1); });
    }
  }
  EXPECT_EQ(n.load(), N);
}

TEST(BlockingPoolTest, AdderN) {
  constexpr int N = 42;
  std::atomic n = 0;
  {
    ThreadPool pool(8);
    for (int i = 0; i < 42; i++) {
      pool.spawn([&] { n.fetch_add(1); });
    }
  }
  EXPECT_EQ(n.load(), N);
}
}  // namespace KFC
