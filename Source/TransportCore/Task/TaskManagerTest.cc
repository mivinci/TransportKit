#include "TransportCore/Task/TaskManager.h"
#include "gtest/gtest.h"

namespace TransportCore {
class TaskManagerTest : public testing::Test {};

TEST_F(TaskManagerTest, GenTaskId) {
  constexpr int base = 10, span = 1;
  TaskManager mgr(base, span);
  for (int i = 0; i < base; i++) {
    Task::Context context;
    context.kind = Task::Kind::kTransportCoreTaskKindPlain;
    const int32_t id = mgr.CreateTask(context);
    EXPECT_EQ(id / base,
              static_cast<int32_t>(Task::Kind::kTransportCoreTaskKindPlain));
    EXPECT_EQ(id % base, i);
  }
}

}  // namespace TransportCore
