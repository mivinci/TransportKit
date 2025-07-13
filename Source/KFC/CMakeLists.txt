SET(Headers
  Addr.h
  Async.h
  Assert.h
  Bits.h
  BlockingPool.h
  Clock.h
  Condvar.h
  Endian.h
  Exception.h
  Exchange.h
  Format.h
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
  Span.h
  Sleep.h
  String.h
  Test.h
  Trace.h
  Time.h
  Ticker.h
  Thread.h
  OneOf.h
  URL.h
  WaitGroup.h
)

SET(Sources
  Addr.cc
  Async.cc
  BlockingPool.cc
  Clock.cc
  Exception.cc
  IOBuf.cc
  Log.cc
  Time.cc
  Thread.cc
  String.cc
  Format.cc
  URL.cc
)

SET(TestSources
  AddrTest.cc
  AsyncTest.cc
  LogTest.cc
  ThreadTest.cc
  RefTest.cc
  StringTest.cc
  TraceTest.cc
  TimeTest.cc
  BlockingPoolTest.cc
  ResultTest.cc
  OneOfTest.cc
)

SET(Files ${Headers} ${Sources} ${TestSources})

ADD_LIBRARY(KFC ${Headers} ${Sources})

ADD_EXECUTABLE(KFC_Test ${TestSources})
TARGET_LINK_LIBRARIES(KFC_Test KFC GTest::gtest_main)

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Files})
get_target_property(KFC_CXX_STANDARD KFC CXX_STANDARD)
get_target_property(KFC_Test_CXX_STANDARD KFC_Test CXX_STANDARD)
message(STATUS "[KFC] C艹 standard: ${KFC_CXX_STANDARD}")
message(STATUS "[KFC_Test] C艹 standard: ${KFC_Test_CXX_STANDARD}")
