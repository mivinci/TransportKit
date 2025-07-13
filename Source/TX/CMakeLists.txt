SET(Headers
  # Addr.h
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
  Platform.h
  Runtime.h
  # Result.h
  Ref.h
  RunLoop.h
  RunLoopThread.h
  Socket.h
  Span.h
  Sleep.h
  Trace.h
  Time.h
  Ticker.h
  Thread.h
  WaitGroup.h

  runtime/BlockingPool.h
  runtime/Driver.h
  runtime/Runtime.h
  runtime/Scheduler.h
  runtime/SingleThreadScheduler.h
  runtime/MultiThreadScheduler.h
  Exchange.h

)

SET(Sources
  # Addr.cc
  Log.cc
  RunLoop.cc
  Time.cc

  runtime/BlockingPool.cc
  Thread.cc
)

SET(TestSources
  # AddrTest.cc
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

ADD_EXECUTABLE(TX_Test ${TestSources})
TARGET_LINK_LIBRARIES(TX_Test TX GTest::gtest_main)

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${Files}
)
