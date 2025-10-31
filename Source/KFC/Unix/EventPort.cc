#include "KFC/Unix/EventPort.h"
#include <sys/fcntl.h>

KFC_NAMESPACE_BEG

#if KFC_USE_KQUEUE
UnixEventPort::UnixEventPort() : m_timer(Time::now()) {
  int kqueueFd;
#ifdef __NetBSD__
  kqueueFd = kqueue1();
  KFC_CHECK_SYSCALL(kqueueFd > 0);
#else
  kqueueFd = kqueue();
  KFC_CHECK_SYSCALL(kqueueFd > 0);
  KFC_CHECK_SYSCALL(fcntl(kqueueFd, F_SETFD, FD_CLOEXEC));
#endif

  struct kevent event; // NOLINT(*-pro-type-member-init)
  EV_SET(&event, 0, EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0, nullptr);
  KFC_CHECK_SYSCALL(kevent(kqueueFd, &event, 1, nullptr, 0, nullptr));
  m_kqueueFd = kqueueFd;
}

void UnixEventPort::wake() const {
  struct kevent event; // NOLINT(*-pro-type-member-init)
  EV_SET(&event, 0, EVFILT_USER, 0, NOTE_TRIGGER, 0, nullptr);
  KFC_CHECK_SYSCALL(kevent(m_kqueueFd, &event, 1, nullptr, 0, nullptr));
}

bool UnixEventPort::poll() {
  struct timespec ts, *pts = nullptr; // NOLINT(*-pro-type-member-init)
  KFC_IF_SOME_CONST(t, m_timer.advanceTo(Time::now())) {
    const Clock::TimePoint tp = t.toTimePoint();
    ts.tv_sec = tp.sec;
    ts.tv_nsec = tp.nsec;
    pts = &ts;
  }
  return doKqueueWait(pts);
}

bool UnixEventPort::doKqueueWait(const struct timespec *timeout) const {
  struct kevent events[16];
  int n = kevent(m_kqueueFd, nullptr, 0, events, sizeOf(events), timeout);
  bool woken = false;
  FdObserver *observer = nullptr;

  if (n < 0) {
    const int err = errno;
    if (err == EINTR) {
      n = 0;
    } else {
      KFC_CHECK(n < 0, "kevent errno: %d", err);
    }
  }

  for (int i = 0; i < n; i++) {
    switch (events[i].filter) {
    default:
      KFC_THROW_FATAL(Exception::Syscall, "Unexpected %d", events[i].filter);
#ifdef EVFILT_EXCEPT
    case EVFILT_EXCEPT:
#endif
    case EVFILT_READ:
    case EVFILT_WRITE:
      observer = static_cast<FdObserver *>(events[i].udata);
      observer->fire(events + i);
      break;
    case EVFILT_USER:
      woken = true;
      break;
    }
  }
  return woken;
}

UnixEventPort::FdObserver::FdObserver(UnixEventPort &port, const int fd, const Flag flags)
    : m_port(port), m_fd(fd), m_flags(flags) {
  struct kevent events[3];
  int n = 0;
  if (m_flags & Read) EV_SET(&events[n++], m_fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, this);
  if (m_flags & Write) EV_SET(&events[n++], m_fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, this);
#ifdef EVFILT_EXCEPT
  if (m_flags & Urgent)
    EV_SET(&events[n++], m_fd, EVFILT_EXCEPT, EV_ADD | EV_CLEAR, NOTE_OOB, 0, this);
#endif
  KFC_CHECK_SYSCALL(kevent(m_port.m_kqueueFd, events, n, nullptr, 0, nullptr));
}

UnixEventPort::FdObserver::~FdObserver() noexcept(false) {
  struct kevent events[3];
  int n = 0;
  if (m_flags & Read) EV_SET(&events[n++], m_fd, EVFILT_READ, EV_DELETE, 0, 0, this);
  if (m_flags & Write) EV_SET(&events[n++], m_fd, EVFILT_WRITE, EV_DELETE, 0, 0, this);
#ifdef EVFILT_EXCEPT
  if (m_flags & Urgent) EV_SET(&events[n++], m_fd, EVFILT_EXCEPT, EV_DELETE, 0, 0, this);
#endif
  KFC_CHECK_SYSCALL(kevent(m_port.m_kqueueFd, events, n, nullptr, 0, nullptr));
}

void UnixEventPort::FdObserver::fire(const struct kevent *event) {
  switch (event->filter) {
  default:
    // Just to make the compiler happy, the caller of `fire` has handled the default case by
    // throwing an exception
  case EVFILT_READ:
    m_atEnd = !(event->flags & EV_EOF);
    KFC_IF_SOME(r, m_readResolver) {
      r->resolve();
      m_readResolver = None;
    }
    break;
  case EVFILT_WRITE:
    KFC_IF_SOME(r, m_writeResolver) {
      r->resolve();
      m_writeResolver = None;
    }
    break;
  case EVFILT_EXCEPT:
    KFC_IF_SOME(r, m_urgentResolver) {
      r->resolve();
      m_urgentResolver = None;
    }
    break;
  }
}

#elif KFC_USE_EPOLL
#endif

Promise<void> UnixEventPort::FdObserver::whenBecomeReadable() {
  KFC_CHECK(m_flags & Flag::Read, "FdObserver was not set to observe reads");
  auto par = createPromiseAndResolver<void>();
  m_readResolver = std::move(par.resolver);
  return std::move(par.promise);
}

Promise<void> UnixEventPort::FdObserver::whenBecomeWritable() {
  KFC_CHECK(m_flags & Flag::Write, "FdObserver was not set to observe writes");
  auto par = createPromiseAndResolver<void>();
  m_writeResolver = std::move(par.resolver);
  return std::move(par.promise);
}

Promise<void> UnixEventPort::FdObserver::whenUrgentDataAvailable() {
  KFC_CHECK(m_flags & Flag::Urgent, "FdObserver was not set to observe urgent data");
  auto par = createPromiseAndResolver<void>();
  m_urgentResolver = std::move(par.resolver);
  return std::move(par.promise);
}

Timer &UnixEventPort::getTimer() { return m_timer; }

KFC_NAMESPACE_END
