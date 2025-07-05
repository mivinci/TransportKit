ENABLE_TESTING()

SET(TransportCorePublicHeaders
  API/TransportCore.h
  API/TransportCoreBase.h
  API/TransportCoreErrorCode.h
)

SET(TransportCorePrivateHeaders
  Log/Log.h
  Schedule/Scheduler.h
  Schedule/Task.h
  Schedule/TaskManager.h
  Global/Global.h
)

SET(TransportCoreSources
  API/TransportCore.cc
  API/TransportCoreErrorCode.cc
  Schedule/TaskManager.cc
  Schedule/Scheduler.cc

)

SET(TransportCoreTestSources
  API/TransportCoreTest.cc
  Schedule/TaskManagerTest.cc
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
TARGET_LINK_LIBRARIES(TransportCore PRIVATE TX)

ADD_EXECUTABLE(TransportCore_Test ${TransportCoreTestSources})
TARGET_LINK_LIBRARIES(TransportCore_Test TransportCore GTest::gtest_main)
INCLUDE(GoogleTest)
GTEST_DISCOVER_TESTS(TransportCore_Test)

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${TransportCoreAllFiles}
)
