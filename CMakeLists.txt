cmake_minimum_required(VERSION 3.5)

project(piex CXX)

set(CMAKE_CXX_STANDARD 20)


include(CMakePrintHelpers)

# static lib piex
file(GLOB_RECURSE PIEX_SOURCES src/*.cpp)
cmake_print_variables(PIEX_SOURCES)

add_library(piex STATIC ${PIEX_SOURCES})

target_include_directories(piex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


# static lib piexsdl
find_package(SDL2 REQUIRED)

file(GLOB_RECURSE PIEX_SDL_SOURCES sdl_impl/*.cpp)
cmake_print_variables(PIEX_SDL_SOURCES)

add_library(piexsdl STATIC ${PIEX_SDL_SOURCES})

target_link_libraries(piexsdl PUBLIC SDL2)
target_link_libraries(piexsdl PUBLIC piex)
target_include_directories(piexsdl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


# executable piexapp
file(GLOB_RECURSE PIEXAPP_SOURCES demo/*.cpp)
cmake_print_variables(PIEXAPP_SOURCES)

add_executable(piexapp ${PIEXAPP_SOURCES})

target_link_libraries(piexapp piex)
target_link_libraries(piexapp piexsdl)
target_include_directories(piexapp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(piexapp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/sdl_impl)

add_subdirectory(tests)
