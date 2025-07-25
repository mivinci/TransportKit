SET(TransportCorePublicHeaders
  API/TransportCore.h
  API/TransportCoreBase.h
  API/TransportCoreErrorCode.h
  API/TransportCoreOption.h
)

SET(TransportCorePrivateHeaders
  Log/Log.h
  Task/Scheduler.h
  Task/Task.h
  Task/TaskManager.h
  Global/Global.h
  Global/Option.h
  Task/NoopScheduler.h
)

SET(TransportCoreSources
  API/TransportCore.cc
  API/TransportCoreErrorCode.cc
  Task/TaskManager.cc
  Task/Scheduler.cc
  Log/Log.cc
  Global/Global.cc
)

SET(TransportCoreTestSources
  API/TransportCoreTest.cc
  Task/TaskManagerTest.cc
)

SET(TransportCoreAllFiles 
  ${TransportCorePublicHeaders}
  ${TransportCorePrivateHeaders}
  ${TransportCoreSources}
  ${TransportCoreTestSources}
)

ADD_LIBRARY(TransportCore
  ${TransportCorePublicHeaders}
  ${TransportCorePrivateHeaders}
  ${TransportCoreSources}
)
TARGET_LINK_LIBRARIES(TransportCore PRIVATE TX ${TK_LIBRARIES})
TARGET_COMPILE_DEFINITIONS(TransportCore PUBLIC BUILD_TK fvisibility=hidden)

ADD_EXECUTABLE(TransportCore_Test ${TransportCoreTestSources})
TARGET_LINK_LIBRARIES(TransportCore_Test TransportCore GTest::gtest_main)

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${TransportCoreAllFiles}
)
