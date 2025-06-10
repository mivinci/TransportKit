#pragma once
#include "TX/Assert.h"
#include "TX/runtime/Async.h"
#include "TX/runtime/BlockingPool.h"

namespace TX {
static class Scheduler *currentScheduler = nullptr;

class Scheduler {
 public:
  explicit Scheduler(BlockingPool &pool) : tick_(0), blocking_pool_(pool) {}
  virtual ~Scheduler() = default;
  virtual int Schedule(int turn) = 0;

  template <class F>
  Async<ReturnType<F>> SpawnBlocking(F f) {
    co_return co_await blocking_pool_.Spawn(std::move(f));
  }

  class EnterGuard {
   public:
    explicit EnterGuard(Scheduler *scheduler) : scheduler_(scheduler) {}
    ~EnterGuard() {
      TX_ASSERT(currentScheduler == scheduler_);
      currentScheduler = nullptr;
    }

   private:
    Scheduler *scheduler_;
  };

  EnterGuard Enter() {
    TX_ASSERT(currentScheduler == nullptr);
    currentScheduler = this;
    return EnterGuard(this);
  }

  static Scheduler *Current() { return currentScheduler; }

 protected:
  uint32_t tick_;
  BlockingPool &blocking_pool_;
};

}  // namespace TX
