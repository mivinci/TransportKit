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
    TX_ASSERT_SYSCALL(pthread_cond_init(&m_cond, nullptr));
#endif
  }
  void NotifyOne() {
#ifdef _WIN32
    WakeConditionVariable(&cv);
#else
    TX_ASSERT_SYSCALL(pthread_cond_signal(&m_cond));
#endif
  }
  void NotifyAll() {
#ifdef _WIN32
    WakeAllConditionVariable(&cv);
#else
    TX_ASSERT_SYSCALL(pthread_cond_broadcast(&m_cond));
#endif
  }
  template <typename T>
  bool Wait(MutexGuard<T> &guard, Duration timeout = Duration::FOREVER) {
    if (timeout != Duration::FOREVER) {
#ifdef _WIN32
      const DWORD ms = timeout.MilliSeconds();
      const BOOL rc = SleepConditionVariableCS(&cv, &guard.m_lock->cs, ms);
      if (!rc) {
        DWORD err = GetLastError();
        if (err == ERROR_TIMEOUT || err == WAIT_TIMEOUT) return true;
        return false;
      }
#else
      const Clock::TimePoint tp = Time::After(timeout).ToTimePoint();
      const int rc =
          pthread_cond_timedwait(&m_cond, &guard.m_lock->m_mutex,
                                 reinterpret_cast<const timespec *>(&tp));
      if (rc == ETIMEDOUT) return true;
      TX_ASSERT(rc == 0, "rc(%d)，tp(%lu,%lu), mutex(%p)", rc, tp.sec, tp.nsec,
                guard.m_lock->m_mutex);
#endif
    } else {
#ifdef _WIN32
      SleepConditionVariableCS(&cv, &guard.m_lock->cs, INFINITE);
#else
      TX_ASSERT_SYSCALL(pthread_cond_wait(&m_cond, &guard.m_lock->m_mutex));
#endif
    }
    return false;
  }
#ifdef _WIN32
  CONDITION_VARIABLE cv;
#else
  pthread_cond_t m_cond{};
#endif
};
}  // namespace TX
