# Set up library name
set(TK "TransportKit")

# Set up source directory
set(TK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${TK_SOURCE_DIR})
add_definitions(-DTK_SOURCE_DIR="${TK_SOURCE_DIR}")

# Set up TK sub-libraries
add_subdirectory(API)
add_subdirectory(TransportCore)
add_subdirectory(TX)

add_library(${TK} INTERFACE)
target_link_libraries(${TK} INTERFACE API)
