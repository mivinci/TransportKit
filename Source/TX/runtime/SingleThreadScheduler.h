#pragma once
#include "TX/runtime/Scheduler.h"

namespace TX {
class SingleThreadScheduler : public Scheduler {
 public:
  explicit SingleThreadScheduler(BlockingPool &pool) : Scheduler(pool) {}
  ~SingleThreadScheduler() override = default;
  int Schedule(int turn) override {
    tick_++;
    return 0;
  }
};
}  // namespace TX
