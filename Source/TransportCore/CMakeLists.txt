SET(TransportCorePublicHeaders
  API/TransportCore.h
  API/TransportCoreBase.h
  API/TransportCoreErrorCode.h
  API/TransportCoreOption.h
)

SET(TransportCorePrivateHeaders
  Task/Scheduler.h
  Task/Task.h
  Task/TaskManager.h
  Global/Option.h
  Task/NoopScheduler.h
  P2P/Stun.h
)

SET(TransportCoreSources
  API/TransportCore.cc
  API/TransportCoreErrorCode.cc
  Task/TaskManager.cc
  Task/Scheduler.cc
  P2P/Stun.cc
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
TARGET_LINK_LIBRARIES(TransportCore PRIVATE KFC ${TK_LIBRARIES})
TARGET_COMPILE_DEFINITIONS(TransportCore PUBLIC BUILD_TK fvisibility=hidden)

ADD_EXECUTABLE(TransportCore_Test ${TransportCoreTestSources})
TARGET_LINK_LIBRARIES(TransportCore_Test TransportCore)

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TransportCoreAllFiles})
