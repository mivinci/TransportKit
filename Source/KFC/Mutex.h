#pragma once
#include "KFC/Assert.h"
#include "KFC/CopyMove.h"
#include "KFC/Option.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#define KFC_GIVE_UP_GUARD(g) dtor(g)

KFC_NAMESPACE_BEG

template <class T> class MutexGuard;

template <class T> class Mutex {
public:
  explicit Mutex() { init(); }
  Mutex(T &&t) : m_ptr(t) { init(); }

  template <class... Args> Mutex(Args &&...args) : m_ptr(std::forward<Args>(args)...) { init(); }

  ~Mutex() noexcept(false) {
#ifdef _WIN32
    DeleteCriticalSection(&cs);
#else
    KFC_CHECK_SYSCALL(pthread_mutex_destroy(&m_mutex));
#endif
  }

  MutexGuard<T> lock() {
#ifdef _WIN32
    EnterCriticalSection(&cs);
#else
    KFC_CHECK_SYSCALL(pthread_mutex_lock(&m_mutex));
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
      KFC_THROW(KFC::Exception::Kind::Syscall, "pthread_mutex_trylock: EINVAL");
#endif
    return locked ? Some(MutexGuard<T>(this)) : None;
  }

private:
  void init() {
#ifdef _WIN32
    InitializeCriticalSection(&cs);
#else
    pthread_mutexattr_t attr;
    KFC_CHECK_SYSCALL(pthread_mutexattr_init(&attr));
    KFC_CHECK_SYSCALL(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
    KFC_CHECK_SYSCALL(pthread_mutex_init(&m_mutex, &attr));
    KFC_CHECK_SYSCALL(pthread_mutexattr_destroy(&attr));
#endif
  }

  void unlock() {
#ifdef _WIN32
    LeaveCriticalSection(&cs);
#else
    KFC_CHECK_SYSCALL(pthread_mutex_unlock(&m_mutex));
#endif
  }

  T m_ptr;
#ifdef _WIN32
  CRITICAL_SECTION cs;
#else
  pthread_mutex_t m_mutex{};
#endif

  friend MutexGuard<T>;
  friend class Condition;
};

template <class T> class MutexGuard {
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
  }

  ~MutexGuard() noexcept(false) {
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
  Mutex<T> *m_lock;

  friend class Mutex<T>;
  friend class Condition;
};

KFC_NAMESPACE_END
