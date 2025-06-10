#pragma once
#include "TX/runtime/Scheduler.h"

namespace TX {
class MultiThreadScheduler : public Scheduler {
 public:
  ~MultiThreadScheduler() override = default;
};
}  // namespace TX
