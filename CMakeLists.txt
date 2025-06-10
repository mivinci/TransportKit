cmake_minimum_required(VERSION 3.24)
file(READ version TK_VERSION)
project(TransportKit VERSION ${TK_VERSION})

# Set up CMake options
option(TK_STATIC "Build static library" OFF)
option(TK_ENABLE_TRACE "Enable trace" OFF)
option(TK_USE_FMTLIB "Use fmt lib" OFF)

# Set up compile definitions
add_definitions(-DBUILD_TK -Dfvisibility=hidden)
if (TK_USE_FMTLIB)
  add_definitions(-DUSE_FMTLIB)
endif ()
if (TK_ENABLE_TRACE)
  add_definitions(-DENABLE_TRACE)
endif ()

# Set up CMake modules
include(CMake/SetupCXX.cmake)
include(CMake/SetupFmt.cmake)
include(CMake/SetupGTest.cmake)

# Set up TK libraries
add_subdirectory(Source)
