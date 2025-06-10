#include "gtest/gtest.h"
#include "TX/runtime/SingleThreadScheduler.h"

namespace TX {
TEST(SingleThreadSchedulerTest, Schedule) {
  BlockingPool pool(1);
  SingleThreadScheduler scheduler(pool);
  scheduler.Schedule(1);
}
}
