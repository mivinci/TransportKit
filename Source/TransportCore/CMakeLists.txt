ENABLE_TESTING()

SET(TransportCoreHeaders
  API/TransportCore.h
  Log/Log.h
  Task/Task.h
  Task/TaskManager.h
  Task/TaskScheduler.h
)

SET(TransportCoreSources
  API/TransportCore.cc
  Task/Task.cc
  Task/TaskManager.cc
  Task/TaskScheduler.cc
)

SET(TransportCoreTestSources
  Task/TaskManagerTest.cc
)

ADD_LIBRARY(TransportCore ${TransportCoreHeaders} ${TransportCoreSources})
TARGET_LINK_LIBRARIES(TransportCore PRIVATE TX)

ADD_EXECUTABLE(TransportCore_Test ${TransportCoreTestSources})
TARGET_LINK_LIBRARIES(TransportCore_Test TransportCore GTest::gtest_main)
INCLUDE(GoogleTest)
GTEST_DISCOVER_TESTS(TransportCore_Test)
