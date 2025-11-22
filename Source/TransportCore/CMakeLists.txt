set(TransportCorePublicHeaders
  API/TransportCore.h
  API/TransportCoreBase.h
  API/TransportCoreErrorCode.h
  API/TransportCoreOption.h
)

set(TransportCorePrivateHeaders
  task/Scheduler.h
  task/Task.h
  task/TaskId.h
  task/TaskManager.h
  task/NoopScheduler.h
)

set(TransportCoreSources
  API/TransportCore.cc
  API/TransportCoreErrorCode.cc
  task/TaskId.cc
  task/TaskManager.cc
  task/Scheduler.cc
)

set(TransportCoreTestSources
  API/TransportCoreTest.cc
  task/TaskIdTest.cc
  task/TaskManagerTest.cc
)

set(TransportCoreAllFiles
  ${TransportCorePublicHeaders}
  ${TransportCorePrivateHeaders}
  ${TransportCoreSources}
  ${TransportCoreTestSources}
)

add_library(TransportCore
  ${TransportCorePublicHeaders}
  ${TransportCorePrivateHeaders}
  ${TransportCoreSources}
)
target_link_libraries(TransportCore PRIVATE KFC ${TK_LIBRARIES})
target_compile_definitions(TransportCore PUBLIC BUILD_TK fvisibility=hidden)

add_executable(TransportCore_Test ${TransportCoreTestSources})
target_link_libraries(TransportCore_Test TransportCore KFC_test_main)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TransportCoreAllFiles})
