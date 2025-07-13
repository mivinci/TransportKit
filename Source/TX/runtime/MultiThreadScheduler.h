#pragma once
#include "TX/runtime/Scheduler.h"

namespace TX {
class MultiThreadScheduler final : public Scheduler {
 public:
  explicit MultiThreadScheduler(BlockingPool &pool) : Scheduler(pool) {}
  ~MultiThreadScheduler() override = default;
  int Schedule(int turn) override {
    m_tick++;
    return 0;
  }
};
}  // namespace TX
