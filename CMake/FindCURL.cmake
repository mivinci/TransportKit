# Find cURL, the library

# Input variables:
#
# - `CURL_INCLUDE_DIR`:   Path to find cURL header files
# - `CURL_LIBRARY`:       Path to find cURL libraries
# - `CURL_NAME`:          Name of the cURL library
#
# Output variables:
# - `CURL_FOUND`:         System has cURL
# - `CURL_INCLUDE_DIRS`:  The cURL include directories
# - `CURL_LIBRARY_DIRS`:  The cURL library directories
# - `CURL_LIBRARIES`:     The cURL library names
# - `CURL_PC_REQUIRES`:   The cURL pkg-config packages
# - `CURL_VERSION`:       The cURL version
# - `CURL_CFLAGS`:        Required complier flags

set(CURL_PC_REQUIRES "libcurl")

if(UNIX AND
   NOT DEFINED CURL_INCLUDE_DIR AND
   NOT DEFINED CURL_LIBRARY)
  find_package(PkgConfig QUIET)
  pkg_check_modules(CURL ${CURL_PC_REQUIRES})
  # pkg_check_modules 成功我们会得到：
  #
  # <PREFIX>_FOUND	       是否找到模块（1或0）
  # <PREFIX>_INCLUDE_DIRS  头文件目录列表
  # <PREFIX>_LIBRARY_DIRS	 库文件所在目录
  # <PREFIX>_LIBRARIES	   库文件列表（全路径）
  # <PREFIX>_VERSION	     模块版本
  # <PREFIX>_CFLAGS	       完整的编译标志
endif()

if(CURL_FOUND)
  string(REPLACE ";" " " CURL_CFLAGS "${CURL_CFLAGS}")
  message(STATUS "Found CURL (via pkg-config): ${CURL_INCLUDE_DIRS} (found version \"${CURL_VERSION}\")")
else()
  find_path(CURL_INCLUDE_DIR NAMES "curl/curl.h")
  find_library(CURL_LIBRARY NAMES ${CURL_NAME} "curl")

  include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(CURL
      REQUIRED_VARS
        CURL_INCLUDE_DIR
        CURL_LIBRARY
      VERSION_VAR
        CURL_VERSION
  )

  set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIR})
  set(CURL_LIBRARIES    ${CURL_LIBRARY})

  mark_as_advanced(CURL_INCLUDE_DIR CURL_LIBRARY)
endif()
