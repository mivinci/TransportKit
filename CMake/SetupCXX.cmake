set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-stdlib=libc++)
  link_libraries(-stdlib=libc++)
endif()
