#include "TransportCore/Task/TaskManager.h"
#include "KFC/Sleep.h"
#include "KFC/Testing.h"

namespace TransportCore {
class TaskManagerTest : public ::testing::Test {
public:
  static void SetUpTestSuite() {}
  static void TearDownTestSuite() {}
  void SetUp() override { s_numTaskScheduled = 0; }
  void TearDown() override {}

  static int s_numTaskScheduled;
};

int TaskManagerTest::s_numTaskScheduled = 0;

void MockTaskScheduleCallback(uint64_t tick, void *context) {
  TaskManagerTest::s_numTaskScheduled++;
}

TEST_F(TaskManagerTest, TaskId) {
  constexpr int32_t base = kTaskIdBase;
  constexpr TransportCoreTaskKind kind = kTransportCoreTaskKindUnSpec;
  for (int i = 0; i < base; i++) {
    const int32_t id = TaskManager::TaskId::Next(kind, base, 1);
    ASSERT_EQ(id / base - 1, static_cast<int32_t>(kind));
    ASSERT_EQ(id % base, i);
  }
}

TEST_F(TaskManagerTest, Start) {
  TaskManager mgr;
  mgr.Start();
  KFC_SLEEP(3);
  ASSERT_EQ(mgr.GetTick(), 3);
  mgr.Stop();
}

TEST_F(TaskManagerTest, TaskLifeCycle) {
  TaskManager mgr;
  TransportCoreTaskContext context{};
  context.keyid = "abc";
  context.kind = kTransportCoreTaskKindUnSpec;
  context.schedule = MockTaskScheduleCallback;
  const int32_t task_id = mgr.CreateTask(context);
  ASSERT_TRUE(task_id > 0);
  mgr.StartTask(task_id);
  KFC_SLEEP(1);
  mgr.StopTask(task_id);
  ASSERT_EQ(s_numTaskScheduled, 1);
  mgr.Stop();
}

} // namespace TransportCore
