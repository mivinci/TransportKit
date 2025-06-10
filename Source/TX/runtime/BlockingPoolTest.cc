#include "TX/runtime/BlockingPool.h"
#include "gtest/gtest.h"

namespace TX {
TEST(BlockingPoolTest, Simple1) {
  BlockingPool pool(1);
  pool.Spawn([]() {});
  pool.Spawn([]() {});
}

TEST(BlockingPoolTest, Simple2) {
  BlockingPool pool(2);
  pool.Spawn([]() {});
  pool.Spawn([]() {});
}

TEST(BlockingPoolTest, Adder1) {
  int N = 42;
  std::atomic<int> n = 0;
  {
    BlockingPool pool(1);
    for (int i = 0; i < 42; i++) {
      pool.Spawn([&]() { n.fetch_add(1); });
    }
  }
  EXPECT_EQ(n.load(), N);
}

TEST(BlockingPoolTest, Adder2) {
  int N = 42;
  std::atomic<int> n = 0;
  {
    BlockingPool pool(2);
    for (int i = 0; i < 42; i++) {
      pool.Spawn([&]() { n.fetch_add(1); });
    }
  }
  EXPECT_EQ(n.load(), N);
}

TEST(BlockingPoolTest, Adder3) {
  int N = 42;
  std::atomic<int> n = 0;
  {
    BlockingPool pool(3);
    for (int i = 0; i < 42; i++) {
      pool.Spawn([&]() { n.fetch_add(1); });
    }
  }
  EXPECT_EQ(n.load(), N);
}
}  // namespace TX
