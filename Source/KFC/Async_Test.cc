#include "KFC/Async.h"
#include "KFC/Preclude.h"
#include "KFC/Testing.h"
#include "KFC/Thread.h"

KFC_NAMESPACE_BEG

class AsyncTest : public ::testing::Test {};

#define SETUP_TEST_EVENT_LOOP                                                                      \
  EventLoop loop;                                                                                  \
  WaitScope scope(loop)

TEST_F(AsyncTest, EvalVoid) {
  SETUP_TEST_EVENT_LOOP;
  bool done = false;
  Promise<void> p = evaluateLater([&] { done = true; });
  EXPECT_FALSE(done);
  p.wait(scope);
  EXPECT_TRUE(done);
}

TEST_F(AsyncTest, EvalInt) {
  SETUP_TEST_EVENT_LOOP;
  Promise<int> p = evaluateLater([] { return 42; });
  EXPECT_EQ(p.wait(scope), 42);
}

TEST_F(AsyncTest, There) {
  SETUP_TEST_EVENT_LOOP;
  Promise<int> p = evaluateLater([] { return 42; }).then([](const int x) { return x + 1; });
  EXPECT_EQ(p.wait(scope), 43);
}

TEST_F(AsyncTest, ThereVoid) {
  SETUP_TEST_EVENT_LOOP;
  int value = 0;
  Promise<void> p = evaluateLater([] { return 42; }).then([&](const int x) { value = x; });
  p.wait(scope);
  EXPECT_EQ(value, 42);
}

TEST_F(AsyncTest, ThereThere) {
  SETUP_TEST_EVENT_LOOP;
  Promise<int> p = evaluateLater([] { return 42; }) //
                       .then([](const int x) { return x + 1; })
                       .then([](const int x) { return x + 2; });
  EXPECT_EQ(p.wait(scope), 45);
}

TEST_F(AsyncTest, Chain) {
  SETUP_TEST_EVENT_LOOP;
  Promise<int> p1 = evaluateLater([] { return 42; });
  Promise<int> p2 = evaluateLater([] { return 24; });
  auto p3 = p1.then([&](const int x) { //
    return p2.then([x](const int y) { return x + y; });
  });
  EXPECT_EQ(p3.wait(scope), 66);
}

TEST_F(AsyncTest, Exception) {
  SETUP_TEST_EVENT_LOOP;
  Promise<void> p = evaluateLater([] { throw std::runtime_error("test"); });
  EXPECT_THROW(p.wait(scope), Exception);
}

TEST_F(AsyncTest, Discard) {
  SETUP_TEST_EVENT_LOOP;
  bool done = false;
  Promise<int> p = evaluateLater([&] {
    done = true;
    return 42;
  });
  EXPECT_FALSE(done);
  p.discard().wait(scope);
  EXPECT_TRUE(done);
}

TEST_F(AsyncTest, Resolver) {
  SETUP_TEST_EVENT_LOOP;
  auto par = createPromiseAndResolver<int>();
  EXPECT_TRUE(par.resolver->isWaiting());
  par.resolver->resolve(42);
  EXPECT_FALSE(par.resolver->isWaiting());
  EXPECT_EQ(par.promise.wait(scope), 42);
}

TEST_F(AsyncTest, ResolverVoid) {
  SETUP_TEST_EVENT_LOOP;
  auto par = createPromiseAndResolver<void>();
  EXPECT_TRUE(par.resolver->isWaiting());
  par.resolver->resolve();
  EXPECT_FALSE(par.resolver->isWaiting());
  par.promise.wait(scope);
}

TEST_F(AsyncTest, ResolverDistroyedWithoutResolvingPromise) {
  SETUP_TEST_EVENT_LOOP;
  auto par = createPromiseAndResolver<int>();
  dtor(par.resolver);
  EXPECT_THROW(par.promise.wait(scope), Exception);
}

TEST_F(AsyncTest, InThreadExecuteAsync) {
  SETUP_TEST_EVENT_LOOP;
  Ref<Executor> executor = getCurrentThreadExecutor();
  Promise<int> p = executor->executeAsync([] { return 42; });
  EXPECT_EQ(p.wait(scope), 42);
}

TEST_F(AsyncTest, InThreadExecuteAsyncException) {
  SETUP_TEST_EVENT_LOOP;
  Ref<Executor> executor = getCurrentThreadExecutor();
  Promise<void> p = executor->executeAsync([] { throw std::runtime_error("test"); });
  EXPECT_THROW(p.wait(scope), Exception);
}

TEST_F(AsyncTest, InThreadExecuteSync) {
  SETUP_TEST_EVENT_LOOP;
  Ref<Executor> executor = getCurrentThreadExecutor();
  const int p = executor->executeSync([] { return 42; });
  EXPECT_EQ(p, 42);
}

TEST_F(AsyncTest, InThreadExecuteSyncException) {
  SETUP_TEST_EVENT_LOOP;
  Ref<Executor> executor = getCurrentThreadExecutor();
  EXPECT_ANY_THROW(executor->executeSync([] { throw std::runtime_error("test"); }));
}

TEST_F(AsyncTest, CrossThreadExecuteAsync) {
  SETUP_TEST_EVENT_LOOP;
  Condition cv;
  Mutex<Option<Ref<Executor>>> executor;
  Own<_::PromiseResolver<int>> resolver;

  Thread t([&] {
    SETUP_TEST_EVENT_LOOP;
    auto par = createPromiseAndResolver<int>();
    resolver = std::move(par.resolver);
    *executor.lock() = getCurrentThreadExecutor();
    cv.notifyOne();
    EXPECT_EQ(par.promise.wait(scope), 42);
  });

  auto guard = executor.lock();
  cv.wait(guard);
  Promise<void> p = guard->unwrap()->executeAsync([&] { resolver->resolve(42); });
  p.wait(scope);
}

TEST_F(AsyncTest, CrossThreadExecuteAsyncException) {
  SETUP_TEST_EVENT_LOOP;
  Condition cv;
  Mutex<Option<Ref<Executor>>> executor;
  Own<_::PromiseResolver<int>> resolver;

  Thread t([&] {
    SETUP_TEST_EVENT_LOOP;
    auto par = createPromiseAndResolver<int>();
    resolver = std::move(par.resolver);
    *executor.lock() = getCurrentThreadExecutor();
    cv.notifyOne();
    EXPECT_THROW(par.promise.wait(scope), Exception);
  });

  auto guard = executor.lock();
  cv.wait(guard);
  Promise<void> p = guard->unwrap()->executeAsync(
      [&] { resolver->resolve(KFC_EXCEPTION(KFC::Exception::Kind::Logic)); });
  p.wait(scope);
}

TEST_F(AsyncTest, CrossThreadExecuteSync) {
  SETUP_TEST_EVENT_LOOP;
  Condition cv;
  Mutex<Option<Ref<Executor>>> executor;
  Own<_::PromiseResolver<int>> resolver;

  Thread t([&] {
    SETUP_TEST_EVENT_LOOP;
    auto par = createPromiseAndResolver<int>();
    resolver = std::move(par.resolver);
    *executor.lock() = getCurrentThreadExecutor();
    cv.notifyOne();
    EXPECT_EQ(par.promise.wait(scope), 114514);
  });

  auto guard = executor.lock();
  cv.wait(guard);
  const int n = guard->unwrap()->executeSync([&] {
    resolver->resolve(114514);
    return 42;
  });
  EXPECT_EQ(n, 42);
}

KFC_NAMESPACE_END
