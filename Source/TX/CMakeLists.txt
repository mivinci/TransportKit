enable_testing()

set(TX_HEADERS
  Addr.h
  Assert.h
  Bits.h
  Clock.h
  Condvar.h
  Endian.h
  Exception.h
  Format.h
  Function.h
  Log.h
  Memory.h
  Mutex.h
  Option.h
  Own.h
  Path.h
  Platform.h
  Runtime.h
  Result.h
  Ref.h
  RunLoop.h
  Socket.h
  Span.h
  String.h
  Trace.h
  Time.h
  Thread.h
  WaitGroup.h

  #  fs/File.h
#  io/Error.h
#  net/DNS.h
#  net/TCP.h
#  net/UDP.h
#  net/UNIX.h
  runtime/BlockingPool.h
  runtime/Driver.h
  runtime/EventSource.h
  runtime/EventLoop.h
  runtime/Runtime.h
  runtime/Scheduler.h
  runtime/SingleThreadScheduler.h
  runtime/MultiThreadScheduler.h
  runtime/Task.h
)

set(TX_SOURCES
  Addr.cc
  Log.cc
  RunLoop.cc

#  runtime/BlockingPool.cc
)

set(TX_TEST_SOURCES
  AddrTest.cc
  LogTest.cc
  ThreadTest.cc
  RefTest.cc
  RunLoopTest.cc
  TraceTest.cc
  TimeTest.cc

  #  fs/FileTest.cc
#  runtime/BlockingPoolTest.cc
#  runtime/SingleThreadSchedulerTest.cc
#  runtime/RuntimeTest.cc
)

add_library(TX ${TX_HEADERS} ${TX_SOURCES})
target_link_libraries(TX PUBLIC fmt::fmt)

add_executable(TX_Test ${TX_TEST_SOURCES})
target_link_libraries(TX_Test TX GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(TX_Test)
