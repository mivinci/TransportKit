ENABLE_TESTING()

SET(Headers
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
  RunLoopThread.h
  Socket.h
  Span.h
  String.h
  Trace.h
  Time.h
  Thread.h
  WaitGroup.h

  #  net/DNS.h
  #  net/TCP.h
  #  net/UDP.h
  #  net/UNIX.h
  runtime/BlockingPool.h
  runtime/Driver.h
  runtime/Runtime.h
  runtime/Scheduler.h
  runtime/SingleThreadScheduler.h
  runtime/MultiThreadScheduler.h
  runtime/Task.h
)

SET(Sources
  Addr.cc
  Log.cc
  RunLoop.cc

  runtime/BlockingPool.cc
)

SET(TestSources
  AddrTest.cc
  LogTest.cc
  ThreadTest.cc
  RefTest.cc
  RunLoopTest.cc
  TraceTest.cc
  TimeTest.cc

  runtime/BlockingPoolTest.cc
  runtime/RuntimeTest.cc
  runtime/SingleThreadSchedulerTest.cc
)

SET(Files ${Headers} ${Sources} ${TestSources})

ADD_LIBRARY(TX ${Headers} ${Sources})
TARGET_LINK_LIBRARIES(TX PUBLIC fmt::fmt)

ADD_EXECUTABLE(TX_Test ${TestSources})
TARGET_LINK_LIBRARIES(TX_Test TX GTest::gtest_main)

INCLUDE(GoogleTest)
GTEST_DISCOVER_TESTS(TX_Test)

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${Files}
)
