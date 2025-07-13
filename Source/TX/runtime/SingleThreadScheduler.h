#pragma once
#include "TX/runtime/Scheduler.h"

namespace TX {
class SingleThreadScheduler final : public Scheduler {
 public:
  explicit SingleThreadScheduler(BlockingPool &pool) : Scheduler(pool) {}
  ~SingleThreadScheduler() override = default;
  int Schedule(int turn) override {
    m_tick++;
    return 0;
  }
};
}  // namespace TX
