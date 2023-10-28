cmake_minimum_required(VERSION 3.5)

project(piex CXX)

set(CMAKE_CXX_STANDARD 20)


include(CMakePrintHelpers)

# static lib
file(GLOB_RECURSE PIEXLIB_SOURCES src/*.cpp)
cmake_print_variables(PIEXLIB_SOURCES)

add_library(piexlib STATIC ${PIEXLIB_SOURCES})
target_include_directories(piexlib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


# app
file(GLOB_RECURSE PIEXAPP_SOURCES demo/*.cpp)
cmake_print_variables(PIEXAPP_SOURCES)

add_executable(piexapp ${PIEXAPP_SOURCES})
target_link_libraries(piexapp piexlib)
target_include_directories(piexapp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)
