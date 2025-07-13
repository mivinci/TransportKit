#include "KFC/Async.h"
#include "KFC/Test.h"

namespace KFC {
class AsyncTest : public ::testing::Test {};

#define SETUP_TEST_EVENT_LOOP                                                                      \
  EventLoop loop;                                                                                  \
  WaitScope scope(loop)

TEST_F(AsyncTest, EvalVoid) {
  SETUP_TEST_EVENT_LOOP;
  bool done = false;
  Promise<void> p = evalLater([&] { done = true; });
  EXPECT_FALSE(done);
  p.wait(scope);
  EXPECT_TRUE(done);
}

TEST_F(AsyncTest, EvalInt) {
  SETUP_TEST_EVENT_LOOP;
  Promise<int> p = evalLater([] { return 42; });
  EXPECT_EQ(p.wait(scope), 42);
}

TEST_F(AsyncTest, There) {
  SETUP_TEST_EVENT_LOOP;
  Promise<int> p = evalLater([] { return 42; }).then([](const int x) { return x + 1; });
  EXPECT_EQ(p.wait(scope), 43);
}

TEST_F(AsyncTest, ThereVoid) {
  SETUP_TEST_EVENT_LOOP;
  int value = 0;
  Promise<void> p = evalLater([] { return 42; }).then([&](const int x) { value = x; });
  p.wait(scope);
  EXPECT_EQ(value, 42);
}

TEST_F(AsyncTest, ThereThere) {
  SETUP_TEST_EVENT_LOOP;
  Promise<int> p = evalLater([] { return 42; }) //
                       .then([](const int x) { return x + 1; })
                       .then([](const int x) { return x + 2; });
  EXPECT_EQ(p.wait(scope), 45);
}

TEST_F(AsyncTest, Chain) {
  SETUP_TEST_EVENT_LOOP;
  Promise<int> p1 = evalLater([] { return 42; });
  Promise<int> p2 = evalLater([] { return 24; });
  auto p3 = p1.then([&](const int x) { //
    return p2.then([x](const int y) { return x + y; });
  });
  EXPECT_EQ(p3.wait(scope), 66);
}

TEST_F(AsyncTest, Exception) {
  SETUP_TEST_EVENT_LOOP;
  Promise<void> p = evalLater([] { throw std::runtime_error("test"); });
  EXPECT_THROW(p.wait(scope), Exception);
}

TEST_F(AsyncTest, Discard) {
  SETUP_TEST_EVENT_LOOP;
  bool done = false;
  Promise<int> p = evalLater([&] {
    done = true;
    return 42;
  });
  EXPECT_FALSE(done);
  p.discard().wait(scope);
  EXPECT_TRUE(done);
}

} // namespace KFC
