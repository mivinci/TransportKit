set(Headers
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

set(Sources
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

set(TestSources
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
)

if (UNIX)
  set(UnixHeaders
    Unix/EventPort.h
    Unix/OwnFd.h
  )
  set(UnixSources
    Unix/EventPort.cc
    Unix/OwnFd.cc
  )
  set(UnixTestSources
    Unix/EventPortTest.cc
  )
  list(APPEND Headers ${UnixHeaders})
  list(APPEND Sources ${UnixSources})
  list(APPEND TestSources ${UnixTestSources})
elseif (WIN32)

endif (UNIX)

set(Files ${Headers} ${Sources} ${TestSources})

add_library(KFC ${Headers} ${Sources})

add_library(KFC_test_main TestingMain.cc)

add_executable(KFC_Test ${TestSources})
target_link_libraries(KFC_Test KFC GTest KFC_test_main)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Files})
get_target_property(KFC_CXX_STANDARD KFC CXX_STANDARD)
get_target_property(KFC_Test_CXX_STANDARD KFC_Test CXX_STANDARD)
message(STATUS "[KFC] C艹 standard: ${KFC_CXX_STANDARD}")
message(STATUS "[KFC_Test] C艹 standard: ${KFC_Test_CXX_STANDARD}")
