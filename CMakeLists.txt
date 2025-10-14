cmake_minimum_required(VERSION 3.24)
file(READ version TK_VERSION)
project(TransportKit VERSION ${TK_VERSION})

# Set up C/C艹 standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set up CMake options
option(TK_STATIC "Build static library" OFF)
option(TK_ENABLE_TRACE "Enable tracing" OFF)
option(TK_ENABLE_HTTP "Enable HTTP (with TLS)" ON)
option(TK_ENABLE_P2P "Enable P2P" OFF)
option(TK_USE_CURL "Use libcurl for HTTP data transmission" ON)
option(TK_USE_FMT "Use fmt for string formatting" OFF)
option(TK_USE_GTEST "Use GTest for unit-testing" ON)

# Set up CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
include(CMake/Globals.cmake)

# Set up dependencies
set(TK_INCLUDE_DIRS "")
set(TK_LIBRARY_DIRS "")
set(TK_LIBRARIES "")

if(TK_ENABLE_HTTP)
  if(TK_USE_CURL)
    find_package(CURL REQUIRED)
    list(APPEND TK_LIBRARIES ${CURL_LIBRARIES})
    list(APPEND TK_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})
    list(APPEND TK_LIBRARY_DIRS ${CURL_LIBRARY_DIRS})
  endif()
endif()

if(TK_USE_GTEST)
  find_package(GTEST REQUIRED)
  list(APPEND TK_LIBRARIES ${GTEST_LIBRARIES})
  list(APPEND TK_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS})
  list(APPEND TK_LIBRARY_DIRS ${GTEST_LIBRARY_DIRS})
endif()

include_directories(${TK_INCLUDE_DIRS})
link_directories(${TK_LIBRARY_DIRS})

set(BUILD_INFO "\
Build.CMake.Version:        ${CMAKE_VERSION}
Build.Platform.Arch:        ${CMAKE_HOST_SYSTEM_PROCESSOR}
Build.Platform.OS:          ${CMAKE_SYSTEM_NAME}
Build.Compile.IncludeDirs   ${TK_INCLUDE_DIRS}
Build.Compile.LibraryDirs   ${TK_LIBRARY_DIRS}
Build.Compile.Libraries:    ${TK_LIBRARIES}
")
message("============ Build Info ============\n${BUILD_INFO}====================================")
file(WRITE "${CMAKE_BINARY_DIR}/build-info.txt" "${BUILD_INFO}")

# Set up compile definitions
add_definitions_if_option(TK_ENABLE_TRACE  ENABLE_TRACE)
add_definitions_if_option(TK_ENABLE_HTTP   ENABLE_HTTP)
add_definitions_if_option(TK_ENABLE_P2P    ENABLE_P2P)
add_definitions_if_option(TK_USE_FMT       USE_FMT)
add_definitions_if_option(TK_USE_GTEST     USE_GTEST)

# Set up TK sources
add_subdirectory(Source)
