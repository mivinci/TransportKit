SET(Headers
  Addr.h
  Async.h
  Assert.h
  Bits.h
  Clock.h
  Condition.h
  CopyMove.h
  Disposer.h
  Endian.h
  Exception.h
  Exchange.h
  Format.h
  Function.h
  IOBuf.h
  List.h
  Log.h
  Memory.h
  Mutex.h
  Option.h
  Own.h
  Preclude.h
  Result.h
  RunCatchingExceptions.h
  Ref.h
  RefCounted.h
  Span.h
  StackTrace.h
  Sleep.h
  String.h
  System.h
  Testing.h
  ThreadPool.h
  Trace.h
  Time.h
  Thread.h
  OneOf.h
  URL.h
  WaitGroup.h
)

SET(Sources
  Addr.cc
  Async.cc
  Condition.cc
  Clock.cc
  Disposer.cc
  Exception.cc
  Format.cc
  IOBuf.cc
  Log.cc
  Time.cc
  Thread.cc
  StackTrace.cc
  String.cc
  System.cc
  ThreadPool.cc
  Trace.cc
  URL.cc
)

SET(TestSources
  Addr_Test.cc
  Async_Test.cc
  List_Test.cc
  Log_Test.cc
  Thread_Test.cc
  Ref_Test.cc
  StackTrace_Test.cc
  String_Test.cc
  Trace_Test.cc
  Time_Test.cc
  ThreadPool_Test.cc
  Result_Test.cc
  OneOf_Test.cc
  Own_Test.cc

  Testing.cc
)

IF (UNIX)
  SET(UnixHeaders
    Unix/EventPort.h
    Unix/Fd.h
  )
  SET(UnixSources
    Unix/EventPort.cc
    Unix/Fd.cc
  )
  SET(UnixTestSources
    Unix/EventPort_Test.cc
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
