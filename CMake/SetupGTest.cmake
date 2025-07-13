include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723  # 1.17.0
)
FetchContent_MakeAvailable(googletest)
