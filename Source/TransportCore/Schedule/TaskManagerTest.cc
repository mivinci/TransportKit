#include "TX/RunLoopThread.h"
#include "TransportCore/Schedule/TaskManager.h"
#include "gtest/gtest.h"

namespace TransportCore {
static TX::Own<TX::RunLoopThread> run_loop_thread;
static int num_scheduled;

class TaskManagerTest : public ::testing::Test {
 public:
  static void SetUpTestSuite() {
    run_loop_thread = TX::RunLoopThread::Spawn("TransportCoreTestRunLoop");
  }
  static void TearDownTestSuite() { auto _ = std::move(run_loop_thread); }
  void SetUp() override { num_scheduled = 0; }
  void TearDown() override {}
};

void MockTaskScheduleCallback(uint64_t tick, void *context) { num_scheduled++; }

TEST_F(TaskManagerTest, GenTaskId) {
  constexpr int32_t base = kTaskIdBase;
  TaskManager mgr(run_loop_thread->GetRunLoop());
  for (int i = 0; i < base; i++) {
    TransportCoreTaskContext context;
    context.kind = kTransportCoreTaskKindUnSpec;
    const int32_t id = mgr.CreateTask(context);
    ASSERT_EQ((id / base) - 1,
              static_cast<int32_t>(kTransportCoreTaskKindUnSpec));
    ASSERT_EQ(id % base, i);
  }
}

TEST_F(TaskManagerTest, Start) {
  TaskManager mgr(run_loop_thread->GetRunLoop());
  mgr.Start();
  sleep(3);
  mgr.Stop();
  ASSERT_EQ(mgr.GetTick(), 3);
}

TEST_F(TaskManagerTest, TaskLifeCycle) {
  TaskManager mgr(run_loop_thread->GetRunLoop());
  TransportCoreTaskContext context;
  context.kind = kTransportCoreTaskKindUnSpec;
  context.schedule = MockTaskScheduleCallback;
  const int32_t task_id = mgr.CreateTask(context);
  ASSERT_TRUE(task_id > 0);
  mgr.StartTask(task_id);
  sleep(1);
  mgr.StopTask(task_id);
  ASSERT_EQ(num_scheduled, 1);
}

}  // namespace TransportCore
