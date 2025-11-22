#pragma once

#include "KFC/Assert.h"
#include "KFC/Mutex.h"
#include "KFC/Preclude.h"
#include "KFC/Time.h"

KFC_NAMESPACE_BEG

class Condvar {
public:
  explicit Condvar();

  void notifyOne();
  void notifyAll();

  template <typename T> bool wait(MutexGuard<T> &guard, Duration timeout = Duration::FOREVER) {
    if (timeout != Duration::FOREVER) {
#ifdef _WIN32
      const DWORD ms = timeout.MilliSeconds();
      const BOOL rc = SleepConditionVariableCS(&cv, &guard.m_lock->cs, ms);
      if (!rc) {
        DWORD err = GetLastError();
        if (err == ERROR_TIMEOUT || err == WAIT_TIMEOUT) return false;
        return true;
      }
#else
      const Clock::TimePoint tp = Time::after(timeout).toTimePoint();
      const int rc = pthread_cond_timedwait(&m_cond, &guard.m_lock->m_mutex,
                                            reinterpret_cast<const timespec *>(&tp));
      if (rc == ETIMEDOUT) return false;
      KFC_CHECK(rc == 0, "rc(%d)，tp(%lu,%lu), mutex(%p)", rc, tp.sec, tp.nsec,
                guard.m_lock->m_mutex);
#endif
    } else {
#ifdef _WIN32
      SleepConditionVariableCS(&cv, &guard.m_lock->cs, INFINITE);
#else
      KFC_CHECK_SYSCALL(pthread_cond_wait(&m_cond, &guard.m_lock->m_mutex));
#endif
    }
    return true;
  }

private:
#ifdef _WIN32
  CONDITION_VARIABLE cv;
#else
  pthread_cond_t m_cond;
#endif
};

KFC_NAMESPACE_END