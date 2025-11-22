SET(Headers
  Addr.h
  Async.h
  Assert.h
  Bits.h
  Clock.h
  Condvar.h
  CopyMove.h
  Disposer.h
  Endian.h
  Exception.h
  Exchange.h
  Format.h
  Function.h
  IOBuf.h
  List.h
  Memory.h
  Mutex.h
  Option.h
  Own.h
  Preclude.h
  Result.h
  RunCatchingExceptions.h
  Ref.h
  RefCounted.h
  ScheduleHandle.h
  Span.h
  StackTrace.h
  Sleep.h
  String.h
  System.h
  Testing.h
  ThreadPool.h
  Trace.h
  Time.h
  Timer.h
  Thread.h
  OneOf.h
  URL.h
  WaitGroup.h
)

SET(Sources
  Addr.cc
  Async.cc
  Bits.cc
  Condvar.cc
  Clock.cc
  Disposer.cc
  Exception.cc
  Format.cc
  IOBuf.cc
  Time.cc
  Timer.cc
  Thread.cc
  StackTrace.cc
  String.cc
  System.cc
  ThreadPool.cc
  Trace.cc
  URL.cc
)

SET(TestSources
  AddrTest.cc
  AsyncTest.cc
  BitsTest.cc
  ExceptionTest.cc
  ListTest.cc
  ThreadTest.cc
  RefTest.cc
  StackTraceTest.cc
  StringTest.cc
  TraceTest.cc
  TimeTest.cc
  ThreadPoolTest.cc
  ResultTest.cc
  OneOfTest.cc
  OwnTest.cc
  TestingMain.cc
)

IF (UNIX)
  SET(UnixHeaders
    Unix/EventPort.h
    Unix/OwnFd.h
  )
  SET(UnixSources
    Unix/EventPort.cc
    Unix/OwnFd.cc
  )
  SET(UnixTestSources
    Unix/EventPortTest.cc
  )
  LIST(APPEND Headers ${UnixHeaders})
  LIST(APPEND Sources ${UnixSources})
  LIST(APPEND TestSources ${UnixTestSources})
ELSEIF (WIN32)

ENDIF (UNIX)

SET(Files ${Headers} ${Sources} ${TestSources})

ADD_LIBRARY(KFC ${Headers} ${Sources})

ADD_EXECUTABLE(KFC_Test ${TestSources})
TARGET_LINK_LIBRARIES(KFC_Test KFC GTest)

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Files})
GET_TARGET_PROPERTY(KFC_CXX_STANDARD KFC CXX_STANDARD)
GET_TARGET_PROPERTY(KFC_Test_CXX_STANDARD KFC_Test CXX_STANDARD)
MESSAGE(STATUS "[KFC] C艹 standard: ${KFC_CXX_STANDARD}")
MESSAGE(STATUS "[KFC_Test] C艹 standard: ${KFC_Test_CXX_STANDARD}")
