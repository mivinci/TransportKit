#pragma once
#include "TX/runtime/BlockingPool.h"
#include "TX/runtime/MultiThreadScheduler.h"
#include "TX/runtime/Scheduler.h"
#include "TX/runtime/SingleThreadScheduler.h"

namespace TX {
class Runtime {
 public:
  enum class Mode : int {
    SingleThread,
    MultiThread,
  };

  static Runtime SingleThread() { return Runtime(Mode::SingleThread, 2); }
  static Runtime MultiThread(const int max_threads = 3) {
    // TODO:
    // 1. Env TX_MAX_THREAD
    // 2. The number of CPU cores
    return Runtime(Mode::MultiThread, max_threads);
  }

  Scheduler::EnterGuard Enter() { return scheduler_->Enter(); }

  template <class F>
  void BlockOn(F f) {}

 private:
  explicit Runtime(const Mode mode, const int max_thread)
      : blocking_pool_(max_thread) {
    switch (mode) {
      case Mode::SingleThread:
        scheduler_ = new SingleThreadScheduler(blocking_pool_);
        break;
      case Mode::MultiThread:
        scheduler_ = new MultiThreadScheduler(blocking_pool_);
      default:
        TX_FATAL("Unknown runtime mode %d", mode);
    }
  }

  Own<Scheduler> scheduler_;
  BlockingPool blocking_pool_;
};

template <class F>
Task::Handle<ReturnType<F>> SpawnBlocking(F f) {
  return Scheduler::Current()->SpawnBlocking(f);
}

}  // namespace TX
