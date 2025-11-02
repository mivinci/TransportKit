#include "KFC/Testing.h"
#include "KFC/ThreadPool.h"

KFC_NAMESPACE_BEG

TEST(ThreadPoolTest, Simple1) {
  ThreadPool pool(1);
  pool.submit([] {});
  pool.submit([] {});
}

TEST(ThreadPoolTest, Simple2) {
  ThreadPool pool(2);
  pool.submit([] {});
  pool.submit([] {});
}

TEST(ThreadPoolTest, Adder1) {
  constexpr int N = 42;
  std::atomic<int> n(0);
  {
    ThreadPool pool(1);
    for (int i = 0; i < 42; i++) {
      pool.submit([&] { n.fetch_add(1); });
    }
  }
  EXPECT_EQ(n.load(), N);
}

TEST(ThreadPoolTest, Adder2) {
  constexpr int N = 42;
  std::atomic<int> n(0);
  {
    ThreadPool pool(2);
    for (int i = 0; i < 42; i++) {
      pool.submit([&] { n.fetch_add(1); });
    }
  }
  EXPECT_EQ(n.load(), N);
}

TEST(ThreadPoolTest, Adder3) {
  constexpr int N = 42;
  std::atomic<int> n(0);
  {
    ThreadPool pool(3);
    for (int i = 0; i < 42; i++) {
      pool.submit([&] { n.fetch_add(1); });
    }
  }
  EXPECT_EQ(n.load(), N);
}

TEST(ThreadPoolTest, AdderN) {
  constexpr int N = 42;
  std::atomic<int> n(0);
  {
    ThreadPool pool(8);
    for (int i = 0; i < 42; i++) {
      pool.submit([&] { n.fetch_add(1); });
    }
  }
  EXPECT_EQ(n.load(), N);
}

KFC_NAMESPACE_END
