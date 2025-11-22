SET(TransportCorePublicHeaders
  API/TransportCore.h
  API/TransportCoreBase.h
  API/TransportCoreErrorCode.h
  API/TransportCoreOption.h
)

SET(TransportCorePrivateHeaders
  task/Scheduler.h
  task/Task.h
  task/TaskManager.h
  task/NoopScheduler.h
)

SET(TransportCoreSources
  API/TransportCore.cc
  API/TransportCoreErrorCode.cc
  task/TaskManager.cc
  task/Scheduler.cc
)

SET(TransportCoreTestSources
  API/TransportCoreTest.cc
  task/TaskManagerTest.cc
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
TARGET_LINK_LIBRARIES(TransportCore PRIVATE KFC ${TK_LIBRARIES})
TARGET_COMPILE_DEFINITIONS(TransportCore PUBLIC BUILD_TK fvisibility=hidden)

ADD_EXECUTABLE(TransportCore_Test ${TransportCoreTestSources})
TARGET_LINK_LIBRARIES(TransportCore_Test TransportCore)

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TransportCoreAllFiles})
