#pragma once
#include "TX/Assert.h"
#include "TX/runtime/BlockingPool.h"

namespace TX {
static class Scheduler *currentScheduler = nullptr;

class Scheduler {
 public:
  explicit Scheduler(BlockingPool &pool) : m_tick(0), m_blockingPool(pool) {}
  virtual ~Scheduler() = default;
  virtual int Schedule(int) = 0;

  template <class F>
  void SpawnBlocking(F f) {
    return m_blockingPool.Spawn(std::move(f));
  }

  class EnterGuard {
   public:
    explicit EnterGuard(Scheduler *scheduler) : m_prev(currentScheduler) {
      currentScheduler = scheduler;
    }
    ~EnterGuard() { currentScheduler = m_prev; }

   private:
    Scheduler *m_prev;
  };

  EnterGuard Enter() { return EnterGuard(this); }

  static Scheduler *Current() {
    TX_ASSERT(currentScheduler != nullptr);
    return currentScheduler;
  }

 protected:
  uint32_t m_tick;
  BlockingPool &m_blockingPool;
};

}  // namespace TX
