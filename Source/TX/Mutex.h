#pragma once
#include "TX/Assert.h"
#include "TX/Memory.h"
#include "TX/Option.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace TX {
template <typename T>
class MutexGuard;
template <typename T>
class Mutex {
 public:
  explicit Mutex() {
#ifdef _WIN32
    InitializeCriticalSection(&cs);
#else
    pthread_mutexattr_t attr;
    TX_ASSERT_SYSCALL(pthread_mutexattr_init(&attr));
    TX_ASSERT_SYSCALL(
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
    TX_ASSERT_SYSCALL(pthread_mutex_init(&inner_, &attr));
    TX_ASSERT_SYSCALL(pthread_mutexattr_destroy(&attr));
#endif
  }
  Mutex(T &&t) : t_(t) {
#ifdef _WIN32
    InitializeCriticalSection(&cs);
#else
    pthread_mutexattr_t attr;
    TX_ASSERT_SYSCALL(pthread_mutexattr_init(&attr));
    TX_ASSERT_SYSCALL(
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
    TX_ASSERT_SYSCALL(pthread_mutex_init(&inner_, &attr));
    TX_ASSERT_SYSCALL(pthread_mutexattr_destroy(&attr));
#endif
  }
  ~Mutex() {
#ifdef _WIN32
    DeleteCriticalSection(&cs);
#else
    TX_ASSERT_SYSCALL(pthread_mutex_destroy(&inner_));
#endif
  }

  MutexGuard<T> Lock() {
#ifdef _WIN32
    EnterCriticalSection(&cs);
#else
    TX_ASSERT_SYSCALL(pthread_mutex_lock(&inner_));
#endif
    return MutexGuard<T>(this);
  }

  Option<MutexGuard<T>> TryLock() {
    bool locked = false;
#ifdef _WIN32
    locked = TryEnterCriticalSection(&cs);
#else
    int rc = pthread_mutex_trylock(&inner_);
    if (rc == 0)
      locked = true;
    else if (rc == EBUSY)
      locked = false;
    else
      TX_FATAL("pthread_mutex_trylock: EINVAL");
#endif
    return locked ? TX::Some(MutexGuard<T>(this)) : TX::None;
  }

 private:
  void Unlock() {
#ifdef _WIN32
    LeaveCriticalSection(&cs);
#else
    TX_ASSERT_SYSCALL(pthread_mutex_unlock(&inner_));
#endif
  }

 private:
  friend MutexGuard<T>;
  friend class Condvar;
  T t_;
#ifdef _WIN32
  CRITICAL_SECTION cs;
#else
  pthread_mutex_t inner_{};
#endif
};

template <typename T>
class MutexGuard {
 public:
  TX_DISALLOW_COPY(MutexGuard)
  MutexGuard(MutexGuard &&other) noexcept {
    lock_ = other.lock_;
    other.lock_ = nullptr;
  }
  MutexGuard &operator=(MutexGuard &&other) noexcept {
    lock_ = other.lock_;
    other.lock_ = nullptr;
    return *this;
  };
  ~MutexGuard() {
    if (lock_) {
      // A NULL lock_ means that this MutexGuard has been moved to another
      // MutexGuard, in which case we must avoid a double-unlock on lock_.
      lock_->Unlock();
      lock_ = nullptr;
    }
  }
  T &operator*() { return lock_->t_; }
  T *operator->() { return &lock_->t_; }

 private:
  explicit MutexGuard(Mutex<T> *lock) : lock_(lock) {}

  friend class Mutex<T>;
  friend class Condvar;
  Mutex<T> *lock_;
};

}  // namespace TX
