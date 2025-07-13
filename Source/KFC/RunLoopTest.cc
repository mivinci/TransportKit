#include <unordered_set>

#include "KFC/RunLoop.h"
#include "KFC/Test.h"
#include "KFC/WaitScope.h"

namespace KFC {
class RunLoopTest : public testing::Test {
 public:
  explicit RunLoopTest() : m_waitScope(m_runLoop) {}
  void TearDown() override { RunLoop::current().reset(); }

 private:
  RunLoop m_runLoop;
  WaitScope m_waitScope;
};

class MockTimer final : public RunLoop::Timer {
 public:
  explicit MockTimer(const Duration timeout, const Duration period, const Tick repeat)
      : Timer(timeout, period, repeat), m_numTimeout(0) {
    RunLoop::current().addTimer(this);
  }
  ~MockTimer() override { RunLoop::current().removeTimer(this); }
  void onTimeout(const RunLoop &) override { m_numTimeout++; }

  int m_numTimeout;
};

TEST_F(RunLoopTest, Run) {
  RunLoop &loop = RunLoop::current();
  EXPECT_EQ(loop.runCycles(0), kRunLoopStatusFinished);
  EXPECT_EQ(loop.getCycles(), 0);
}
}  // namespace KFC
