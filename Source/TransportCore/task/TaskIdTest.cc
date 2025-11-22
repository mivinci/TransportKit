#include "TransportCore/task/TaskId.h"
#include "KFC/Testing.h"

namespace TransportCore {

TEST(TaskIdTest, Test) {
  constexpr int32_t base = kTaskIdBase;
  constexpr TransportCoreTaskKind kind = kTransportCoreTaskKindUnSpec;
  for (int i = 0; i < base; i++) {
    const int32_t id = TaskId::Next(kind, base, 1);
    ASSERT_EQ(id / base - 1, static_cast<int32_t>(kind));
    ASSERT_EQ(id % base, i);
  }
}

} // namespace TransportCore
