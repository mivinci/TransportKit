#include <unordered_set>

#include "KFC/RunLoop.h"
#include "KFC/Test.h"
#include "KFC/WaitGroup.h"

namespace KFC {
struct RunLoopTest : testing::Test {
  void TearDown() override { RunLoop::current().reset(); }
};

class MockSource final : public RunLoop::Source {
 public:
  explicit MockSource() : m_numScheduled(0), m_numPerformed(0), m_numCanceled(0) {
    RunLoop::current().addSource(this);
  }
  ~MockSource() override { RunLoop::current().removeSource(this); }
  void OnSchedule(const RunLoop &) override { m_numScheduled++; }
  void OnPerform(const RunLoop &) override { m_numPerformed++; }
  void OnCancel(const RunLoop &) override { m_numCanceled++; }

  int m_numScheduled;
  int m_numPerformed;
  int m_numCanceled;
};

class MockTimer final : public RunLoop::Timer {
 public:
  explicit MockTimer(const Duration timeout, const Duration period, const Tick repeat)
      : Timer(timeout, period, repeat), m_numTimeout(0) {
    RunLoop::current().addTimer(this);
  }
  ~MockTimer() override { RunLoop::current().removeTimer(this); }
  void OnTimeout(const RunLoop &) override { m_numTimeout++; }

  int m_numTimeout;
};

TEST_F(RunLoopTest, Current) {
  constexpr int N = 3;
  std::unordered_set<RunLoop *> seen;
  std::vector<Own<Thread>> threads;
  WaitGroup wg(N);
  threads.reserve(N);
  for (int i = 0; i < N; ++i) {
    threads.push_back(Thread::spawn([&] {
      seen.insert(&RunLoop::current());
      seen.insert(&RunLoop::current());
      wg.done();
    }));
  }
  wg.wait();
  EXPECT_EQ(N, seen.size());
}

TEST_F(RunLoopTest, Stop) {
  RunLoop &loop = RunLoop::current();
  MockSource s1;
  loop.performBlock([&] { loop.stop(); });
  EXPECT_EQ(loop.run(), kRunLoopStatusStopped);
}

TEST_F(RunLoopTest, Run) {
  RunLoop &loop = RunLoop::current();
  EXPECT_EQ(loop.runCycles(0), kRunLoopStatusFinished);
  EXPECT_EQ(loop.getCycles(), 0);

  EXPECT_EQ(loop.runCycles(1), kRunLoopStatusFinished);
  EXPECT_EQ(loop.getCycles(), 1);

  MockSource s1;
  EXPECT_EQ(loop.runCycles(1), kRunLoopStatusFinished);
  EXPECT_EQ(s1.m_numScheduled, 1);
  EXPECT_EQ(s1.m_numPerformed, 0);
  s1.signal();
  EXPECT_EQ(loop.runCycles(1), kRunLoopStatusFinished);
  EXPECT_EQ(s1.m_numPerformed, 1);

  const MockTimer t1(10_ms, -1, 0);
  EXPECT_EQ(loop.runCycles(1), kRunLoopStatusFinished);
  EXPECT_EQ(t1.m_numTimeout, 1);

  const MockTimer t2(10_ms, 10_ms, 1);
  EXPECT_EQ(loop.runCycles(1), kRunLoopStatusFinished);
  EXPECT_EQ(t2.m_numTimeout, 1);

  const MockTimer t3(10_ms, 10_ms, 10);
  EXPECT_EQ(loop.runUntil(130_ms), kRunLoopStatusTimeout);
  EXPECT_EQ(t3.m_numTimeout, 11);

  EXPECT_EQ(s1.m_numPerformed, 1);
}

TEST_F(RunLoopTest, RunOnlyTimers) {
  RunLoop &loop = RunLoop::current();
  const MockTimer t1(0, 50_ms, 5);
  EXPECT_EQ(loop.runCycles(6), kRunLoopStatusFinished);
  EXPECT_EQ(t1.m_numTimeout, 6);
}
}  // namespace KFC
