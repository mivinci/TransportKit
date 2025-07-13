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
  RefCounted.h
  RunLoop.h
  Socket.h
  Span.h
  Sleep.h
  String.h
  Test.h
  Trace.h
  Time.h
  Ticker.h
  Thread.h
  ThreadPool.h
  Union.h
  WaitGroup.h
)

SET(Sources
  Addr.cc
  Clock.cc
  IOBuf.cc
  Log.cc
  RunLoop.cc
  Time.cc
  Thread.cc
  ThreadPool.cc
  String.cc
  Format.cc
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
  ThreadPoolTest.cc
  ResultTest.cc
  UnionTest.cc
)

SET(Files ${Headers} ${Sources} ${TestSources})

ADD_LIBRARY(KFC ${Headers} ${Sources})

ADD_EXECUTABLE(KFC_Test ${TestSources})
TARGET_LINK_LIBRARIES(KFC_Test KFC GTest::gtest_main)

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${Files}
)
get_target_property(KFC_CXX_STANDARD KFC CXX_STANDARD)
get_target_property(KFC_Test_CXX_STANDARD KFC_Test CXX_STANDARD)
message(STATUS "[KFC] C艹 standard: ${KFC_CXX_STANDARD}")
message(STATUS "[KFC_Test] C艹 standard: ${KFC_Test_CXX_STANDARD}")
