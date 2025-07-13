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

  Scheduler::EnterGuard Enter() { return m_scheduler->Enter(); }

  template <class F>
  void BlockOn(F f) {
    auto _ = Enter();
    // f();
  }

 private:
  explicit Runtime(const Mode mode, const int max_thread)
      : m_blockingPool(max_thread) {
    switch (mode) {
      case Mode::SingleThread:
        m_scheduler = new SingleThreadScheduler(m_blockingPool);
        break;
      case Mode::MultiThread:
        m_scheduler = new MultiThreadScheduler(m_blockingPool);
      default:
        TX_FATAL("Unknown runtime mode %d", mode);
    }
  }

  Own<Scheduler> m_scheduler;
  BlockingPool m_blockingPool;
};

template <class F>
void SpawnBlocking(F f) {
  return Scheduler::Current()->SpawnBlocking(f);
}

}  // namespace TX
