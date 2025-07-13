#include <unordered_set>

#include "TX/RunLoop.h"
#include "TX/WaitGroup.h"
#include "gtest/gtest.h"

namespace TX {
struct RunLoopTest : testing::Test {
  void TearDown() override { RunLoop::ClearGlobalContext(); }
};

class MockSource final : public RunLoop::Source {
 public:
  explicit MockSource() : m_numScheduled(0), m_numPerformed(0), m_numCanceled(0) {
    RunLoop::Current()->AddSource(this);
  }
  ~MockSource() override { RunLoop::Current()->RemoveSource(this); }
  void OnSchedule(const std::string &scope_name) override {
    m_numScheduled++;
  }
  void OnPerform(const std::string &scope_name) override { m_numPerformed++; }
  void OnCancel(const std::string &scope_name) override { m_numCanceled++; }

  int m_numScheduled;
  int m_numPerformed;
  int m_numCanceled;
};

class MockTimer final : public RunLoop::Timer {
 public:
  explicit MockTimer(const Duration timeout, const Duration period,
                     const uint64_t repeat)
      : Timer(timeout, period, repeat), m_numTimeout(0) {
    RunLoop::Current()->AddTimer(this);
  }
  ~MockTimer() override { RunLoop::Current()->RemoveTimer(this); }
  void OnTimeout(const std::string &scope_name) override { m_numTimeout++; }

  int m_numTimeout;
};

TEST_F(RunLoopTest, Current) {
  constexpr int N = 3;
  std::unordered_set<Ref<RunLoop>> seen;
  std::vector<Own<Thread>> threads;
  WaitGroup wg(N);
  threads.reserve(N);
  for (int i = 0; i < N; ++i) {
    threads.push_back(Thread::Spawn([&] {
      seen.insert(RunLoop::Current());
      seen.insert(RunLoop::Current());
      wg.Done();
    }));
  }
  wg.Wait();
  EXPECT_EQ(N, seen.size());
}

TEST_F(RunLoopTest, Stop) {
  Ref<RunLoop> loop = RunLoop::Current();
  MockSource s1;
  loop->PerformBlock([&]() { loop->Stop(); });
  EXPECT_EQ(loop->Run(), RunLoop::Status::Stopped);
}

TEST_F(RunLoopTest, Run) {
  Ref<RunLoop> loop = RunLoop::Current();
  EXPECT_EQ(loop, RunLoop::Main());
  EXPECT_EQ(loop->Run(0), RunLoop::Status::Finished);
  EXPECT_EQ(loop->GetTick(), 1);

  MockSource s1;
  EXPECT_EQ(loop->Run(0), RunLoop::Status::Finished);
  EXPECT_EQ(s1.m_numScheduled, 1);
  EXPECT_EQ(s1.m_numPerformed, 0);
  s1.Signal();
  EXPECT_EQ(loop->Run(0), RunLoop::Status::Finished);
  EXPECT_EQ(s1.m_numPerformed, 1);

  const MockTimer t1(10_ms, -1, 0);
  EXPECT_EQ(loop->Run(0), RunLoop::Status::Finished);
  EXPECT_EQ(t1.m_numTimeout, 1);

  const MockTimer t2(10_ms, 10_ms, 1);
  EXPECT_EQ(loop->Run(0), RunLoop::Status::Finished);
  EXPECT_EQ(t2.m_numTimeout, 1);

  const MockTimer t3(10_ms, 10_ms, 10);
  EXPECT_EQ(loop->Run(UINT64_MAX, 130_ms), RunLoop::Status::Timeout);
  EXPECT_EQ(t3.m_numTimeout, 11);

  EXPECT_EQ(s1.m_numPerformed, 1);
}

TEST_F(RunLoopTest, RunOnlyTimers) {
  Ref<RunLoop> loop = RunLoop::Current();
  const MockTimer t1(0, 50_ms, 5);
  loop->SetPeriod(1_s);
  EXPECT_EQ(loop->Run(5), RunLoop::Status::Finished);
  EXPECT_EQ(t1.m_numTimeout, 6);
}
}  // namespace TX
