include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(instructions_set instructions_set.cpp)

target_link_libraries(instructions_set
                      PRIVATE
                      GTest::GTest
                      piexlib)

add_test(multiply_gtests instructions_set)

target_include_directories(instructions_set
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../include)
