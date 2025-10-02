#pragma once

#include "KFC/Async.h"
#include "KFC/Preclude.h"
#include "KFC/Timer.h"
#include "KFC/Unix/Fd.h"

#if defined(__APPLE__)
#include <sys/event.h>
#define KFC_USE_KQUEUE 1
#elif defined(__linux__)
#define KFC_USE_EPOLL 1
#else
#include <sys/poll.h>
#define KFC_USE_POLL 1
#endif

KFC_NAMESPACE_BEG

class UnixEventPort final : public EventPort {
public:
  class FdObserver;
  explicit UnixEventPort();
  bool poll() override;
  void wake() const override;

private:
#if KFC_USE_KQUEUE
  bool doKqueueWait(const struct timespec *timeout) const;
  Fd m_kqueueFd;
#elif KFC_USE_EPOLL
  bool doEpollWait(struct timespec *ts);
  Fd m_epollFd;
#endif
  Timer m_timer;
  friend FdObserver;
};

class UnixEventPort::FdObserver {
public:
  enum Flag {
    Read = 1,
    Write = 2,
    Urgent = 4,
  };

  KFC_DISALLOW_COPY_AND_MOVE(FdObserver);
  explicit FdObserver(UnixEventPort &port, int fd, Flag flags);
  ~FdObserver() noexcept(false);
  Promise<void> whenBecomeReadable();
  Promise<void> whenBecomeWritable();
  Promise<void> whenUrgentDataAvailable();

#if KFC_USE_KQUEUE
  void fire(const struct kevent *event);
#elif KFC_USE_EPOLL
#elif KFC_USE_POLL
#endif

private:
  UnixEventPort &m_port;
  int m_fd;
  Flag m_flags;
  Option<bool> m_atEnd;

  Option<Own<_::PromiseResolver<void>>> m_readResolver;
  Option<Own<_::PromiseResolver<void>>> m_writeResolver;
  Option<Own<_::PromiseResolver<void>>> m_urgentResolver;
};

KFC_NAMESPACE_END
