SET(Headers
  Addr.h
  Assert.h
  Bits.h
  Clock.h
  Condvar.h
  Endian.h
  Exception.h
  Exchange.h
  Format.h
  Function.h
  IOBuf.h
  Log.h
  Memory.h
  Mutex.h
  Option.h
  Own.h
  Platform.h
  Result.h
  Ref.h
  RunLoop.h
  RunLoopThread.h
  Socket.h
  Span.h
  Sleep.h
  String.h
  Test.h
  Trace.h
  Time.h
  Ticker.h
  Thread.h
  Union.h
  WaitGroup.h

  runtime/BlockingPool.h
  runtime/Driver.h
  runtime/Runtime.h
  runtime/Scheduler.h
  runtime/SingleThreadScheduler.h
  runtime/MultiThreadScheduler.h
)

SET(Sources
  Addr.cc
  Clock.cc
  IOBuf.cc
  Log.cc
  RunLoop.cc
  RunLoopThread.cc
  Time.cc
  Thread.cc
  String.cc

  runtime/BlockingPool.cc
)

SET(TestSources
  AddrTest.cc
  LogTest.cc
  ThreadTest.cc
  RefTest.cc
  RunLoopTest.cc
  StringTest.cc
  TraceTest.cc
  TimeTest.cc
  ResultTest.cc
  UnionTest.cc

  runtime/BlockingPoolTest.cc
  runtime/RuntimeTest.cc
  runtime/SingleThreadSchedulerTest.cc
)

SET(Files ${Headers} ${Sources} ${TestSources})

ADD_LIBRARY(TX ${Headers} ${Sources})

ADD_EXECUTABLE(TX_Test ${TestSources})
TARGET_LINK_LIBRARIES(TX_Test TX GTest::gtest_main)

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${Files}
)
get_target_property(TX_CXX_STANDARD TX CXX_STANDARD)
get_target_property(TX_Test_CXX_STANDARD TX_Test CXX_STANDARD)
message(STATUS "[TX] C艹 standard: ${TX_CXX_STANDARD}")
message(STATUS "[TX_Test] C艹 standard: ${TX_Test_CXX_STANDARD}")
