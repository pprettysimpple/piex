include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        main
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(rom_tests rom_tests.cpp)

target_link_libraries(rom_tests PRIVATE GTest::gtest_main piexcore piexbasic)
target_include_directories(rom_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
add_test(NAME rom_tests COMMAND rom_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..)

# workaround for Visual Studio
set_target_properties(rom_tests PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

enable_testing()
