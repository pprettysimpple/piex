include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(rom_tests rom_tests.cpp)

target_link_libraries(rom_tests PRIVATE GTest::GTest)
target_link_libraries(rom_tests PRIVATE piexcore)
target_link_libraries(rom_tests PRIVATE piexbasic)

target_include_directories(rom_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_test(NAME rom_tests COMMAND rom_tests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)
enable_testing()
