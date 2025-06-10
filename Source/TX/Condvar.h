#pragma once
#include "TX/Assert.h"
#include "TX/Mutex.h"
#include "TX/Option.h"

namespace TX {
class Condvar {
 public:
  explicit Condvar() {
#ifdef _WIN32
    InitializeConditionVariable(&cv);
#else
    TX_ASSERT_SYSCALL(pthread_cond_init(&cv_, nullptr));
#endif
  }
  void NotifyOne() {
#ifdef _WIN32
    WakeConditionVariable(&cv);
#else
    TX_ASSERT_SYSCALL(pthread_cond_signal(&cv_));
#endif
  }
  void NotifyAll() {
#ifdef _WIN32
    WakeAllConditionVariable(&cv);
#else
    TX_ASSERT_SYSCALL(pthread_cond_broadcast(&cv_));
#endif
  }
  template <typename T>
  bool Wait(MutexGuard<T> &guard, Duration timeout = Duration::FOREVER) {
    if (timeout != Duration::FOREVER) {
#ifdef _WIN32
      DWORD ms = timeout.MilliSeconds();
      SleepConditionVariableCS(&cv, &guard.lock_->cs, ms);
#else
      const Clock::TimePoint tp = Time::After(timeout).ToTimePoint();
      const int rc = pthread_cond_timedwait(
          &cv_, &guard.lock_->inner_, reinterpret_cast<const timespec *>(&tp));
      if (rc == ETIMEDOUT) return true;
      TX_ASSERT(rc == 0, "rc(%d)，tp(%lu,%lu), mutex(%p)", rc, tp.sec, tp.nsec,
                guard.lock_->inner_);
#endif
    } else {
#ifdef _WIN32
      SleepConditionVariableCS(&cv, &guard.lock_->cs, INFINITE);
#else
      TX_ASSERT_SYSCALL(pthread_cond_wait(&cv_, &guard.lock_->inner_));
#endif
    }
    return false;
  }
#ifdef _WIN32
  CONDITION_VARIABLE cv;
#else
  pthread_cond_t cv_{};
#endif
};
}  // namespace TX
