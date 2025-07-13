#pragma once
#include "TX/Assert.h"
#include "TX/runtime/BlockingPool.h"

namespace TX {
static class Scheduler *currentScheduler = nullptr;

class Scheduler {
 public:
  explicit Scheduler(BlockingPool &pool) : tick_(0), blocking_pool_(pool) {}
  virtual ~Scheduler() = default;
  virtual int Schedule(int) = 0;

  template <class F>
  void SpawnBlocking(F f) {
    return blocking_pool_.Spawn(std::move(f));
  }

  class EnterGuard {
   public:
    explicit EnterGuard(Scheduler *scheduler) : prev_(currentScheduler) {
      currentScheduler = scheduler;
    }
    ~EnterGuard() { currentScheduler = prev_; }

   private:
    Scheduler *prev_;
  };

  EnterGuard Enter() { return EnterGuard(this); }

  static Scheduler *Current() {
    TX_ASSERT(currentScheduler != nullptr);
    return currentScheduler;
  }

 protected:
  uint32_t tick_;
  BlockingPool &blocking_pool_;
};

}  // namespace TX
