#pragma once
#include "KFC/Assert.h"
#include "KFC/CopyMove.h"
#include "KFC/Option.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace KFC {
template <typename T> class MutexGuard;
template <typename T> class Mutex {
public:
  explicit Mutex() {
#ifdef _WIN32
    InitializeCriticalSection(&cs);
#else
    pthread_mutexattr_t attr;
    KFC_ASSERT_SYSCALL(pthread_mutexattr_init(&attr));
    KFC_ASSERT_SYSCALL(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
    KFC_ASSERT_SYSCALL(pthread_mutex_init(&m_mutex, &attr));
    KFC_ASSERT_SYSCALL(pthread_mutexattr_destroy(&attr));
#endif
  }
  Mutex(T &&t) : m_ptr(t) {
#ifdef _WIN32
    InitializeCriticalSection(&cs);
#else
    pthread_mutexattr_t attr;
    KFC_ASSERT_SYSCALL(pthread_mutexattr_init(&attr));
    KFC_ASSERT_SYSCALL(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
    KFC_ASSERT_SYSCALL(pthread_mutex_init(&m_mutex, &attr));
    KFC_ASSERT_SYSCALL(pthread_mutexattr_destroy(&attr));
#endif
  }
  ~Mutex() {
#ifdef _WIN32
    DeleteCriticalSection(&cs);
#else
    KFC_ASSERT_SYSCALL(pthread_mutex_destroy(&m_mutex));
#endif
  }

  MutexGuard<T> lock() {
#ifdef _WIN32
    EnterCriticalSection(&cs);
#else
    KFC_ASSERT_SYSCALL(pthread_mutex_lock(&m_mutex));
#endif
    return MutexGuard<T>(this);
  }

  Option<MutexGuard<T>> tryLock() {
    bool locked = false;
#ifdef _WIN32
    locked = TryEnterCriticalSection(&cs);
#else
    const int rc = pthread_mutex_trylock(&m_mutex);
    if (rc == 0)
      locked = true;
    else if (rc == EBUSY)
      locked = false;
    else
      KFC_FATAL("pthread_mutex_trylock: EINVAL");
#endif
    return locked ? Some(MutexGuard<T>(this)) : None;
  }

private:
  void unlock() {
#ifdef _WIN32
    LeaveCriticalSection(&cs);
#else
    KFC_ASSERT_SYSCALL(pthread_mutex_unlock(&m_mutex));
#endif
  }

  friend MutexGuard<T>;
  friend class Condvar;
  T m_ptr;
#ifdef _WIN32
  CRITICAL_SECTION cs;
#else
  pthread_mutex_t m_mutex{};
#endif
};

template <typename T> class MutexGuard {
public:
  KFC_DISALLOW_COPY(MutexGuard)
  MutexGuard(MutexGuard &&other) noexcept {
    m_lock = other.m_lock;
    other.m_lock = nullptr;
  }
  MutexGuard &operator=(MutexGuard &&other) noexcept {
    m_lock = other.m_lock;
    other.m_lock = nullptr;
    return *this;
  };
  ~MutexGuard() {
    if (m_lock) {
      // A NULL m_lock means that this MutexGuard has been moved to another
      // MutexGuard, in which case we must avoid a double-unlock on m_lock.
      m_lock->unlock();
      m_lock = nullptr;
    }
  }
  T &operator*() { return m_lock->m_ptr; }
  T *operator->() { return &m_lock->m_ptr; }

private:
  explicit MutexGuard(Mutex<T> *lock) : m_lock(lock) {}

  friend class Mutex<T>;
  friend class Condvar;
  Mutex<T> *m_lock;
};

} // namespace KFC
