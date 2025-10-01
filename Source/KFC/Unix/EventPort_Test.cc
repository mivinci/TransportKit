#include "KFC/Sleep.h"
#include "KFC/Testing.h"
#include "KFC/Thread.h"
#include "KFC/Unix/EventPort.h"
#include "KFC/Unix/Fd.h"

KFC_NAMESPACE_BEG

#define SETUP_TEST_EVENT_LOOP                                                                      \
  UnixEventPort port;                                                                              \
  EventLoop loop(port);                                                                            \
  WaitScope scope(loop)

TEST(UnixEventPortTest, ReadObserver) {
  SETUP_TEST_EVENT_LOOP;
  int fds[2];
  KFC_CHECK_SYSCALL(pipe(fds));
  const Fd rfd(fds[0]), wfd(fds[1]);
  UnixEventPort::FdObserver observer(port, rfd, UnixEventPort::FdObserver::Read);
  KFC_CHECK_SYSCALL(write(wfd, "abc", 3));
  observer.whenBecomeReadable().wait(scope);
}

TEST(UnixEventPortTest, ReadObserverAsync) {
  SETUP_TEST_EVENT_LOOP;
  int fds[2];
  KFC_CHECK_SYSCALL(pipe(fds));
  const Fd rfd(fds[0]), wfd(fds[1]);
  UnixEventPort::FdObserver observer(port, rfd, UnixEventPort::FdObserver::Read);
  Thread t([&] {
    KFC_SLEEP_US(100 * 1000);
    KFC_CHECK_SYSCALL(write(wfd, "abc", 3));
  });
  observer.whenBecomeReadable().wait(scope);
}

KFC_NAMESPACE_END
