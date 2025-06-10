#pragma once
#include "TX/runtime/Async.h"
#include "TX/runtime/BlockingPool.h"
#include "TX/runtime/Scheduler.h"

namespace TX {
class Runtime {
 public:
  template <class F>
  void BlockOn(F f) {
//    scheduler_->Schedule(-1);
  }

  static Scheduler *CurrentScheduler() { return Scheduler::Current(); }

 private:
  Own<Scheduler> scheduler_;
  BlockingPool blocking_pool_;
};

#define TX_ASYNCIFY(f) (Runtime::CurrentScheduler()->SpawnBlocking(f))
}  // namespace TX
