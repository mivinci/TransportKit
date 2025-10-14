# Find Google Test (gtest), the library

# Input variables:
#
# - `GTEST_INCLUDE_DIR`:   Path to find gtest header files
# - `GTEST_LIBRARY`:       Path to find gtest libraries
# - `GTEST_NAME`:          Name of the gtest library
#
# Output variables:
# - `GTEST_FOUND`:         System has gtest
# - `GTEST_INCLUDE_DIRS`:  The gtest include directories
# - `GTEST_LIBRARY_DIRS`:  The gtest library directories
# - `GTEST_LIBRARIES`:     The gtest library names
# - `GTEST_PC_REQUIRES`:   The gtest pkg-config packages
# - `GTEST_VERSION`:       The gtest version
# - `GTEST_CFLAGS`:        Required compiler flags

set(GTEST_PC_REQUIRES "gtest")

if(UNIX AND
   NOT DEFINED GTEST_INCLUDE_DIR AND
   NOT DEFINED GTEST_LIBRARY)
  find_package(PkgConfig QUIET)
  pkg_check_modules(GTEST ${GTEST_PC_REQUIRES})
  # pkg_check_modules will provide:
  #
  # <PREFIX>_FOUND          Whether the module is found (1 or 0)
  # <PREFIX>_INCLUDE_DIRS   List of include directories
  # <PREFIX>_LIBRARY_DIRS   List of library directories
  # <PREFIX>_LIBRARIES      List of libraries (full paths)
  # <PREFIX>_VERSION        Module version
  # <PREFIX>_CFLAGS         Full compiler flags
endif()

if(GTEST_FOUND)
  string(REPLACE ";" " " GTEST_CFLAGS "${GTEST_CFLAGS}")
  message(STATUS "Found GTEST (via pkg-config): ${GTEST_INCLUDE_DIRS} (found version \"${GTEST_VERSION}\")")
else()
  find_path(GTEST_INCLUDE_DIR NAMES "gtest/gtest.h")
  find_library(GTEST_LIBRARY NAMES ${GTEST_NAME} "gtest")

  include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(GTEST
      REQUIRED_VARS
        GTEST_INCLUDE_DIR
        GTEST_LIBRARY
      VERSION_VAR
        GTEST_VERSION
  )

  set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR})
  set(GTEST_LIBRARIES    ${GTEST_LIBRARY})

  mark_as_advanced(GTEST_INCLUDE_DIR GTEST_LIBRARY)
endif()
