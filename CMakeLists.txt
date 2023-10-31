cmake_minimum_required(VERSION 3.5)

project(piex CXX)

set(CMAKE_CXX_STANDARD 20)


include(CMakePrintHelpers)

# static lib piexcore
file(GLOB_RECURSE PIEXCORE_SOURCES core/*.cpp)
cmake_print_variables(PIEXCORE_SOURCES)

add_library(piexcore STATIC ${PIEXCORE_SOURCES})

target_include_directories(piexcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# static lib piexbasic
file(GLOB_RECURSE PIEXBASIC_SOURCES impl_basic/*.cpp)
cmake_print_variables(PIEXBASIC_SOURCES)

add_library(piexbasic STATIC ${PIEXBASIC_SOURCES})
target_link_libraries(piexbasic PUBLIC piexcore)

target_include_directories(piexbasic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


if(ENABLE_SDL)
    # static lib piexsdl
    find_package(SDL2 REQUIRED)

    file(GLOB_RECURSE PIEX_SDL_SOURCES impl_sdl/*.cpp)
    cmake_print_variables(PIEX_SDL_SOURCES)

    add_library(piexsdl STATIC ${PIEX_SDL_SOURCES})

    target_link_libraries(piexsdl PUBLIC SDL2)
    target_link_libraries(piexsdl PUBLIC piexcore)
    target_include_directories(piexsdl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(piexsdl PRIVATE ${SDL2_INCLUDE_DIRS})


    # executable piexapp
    file(GLOB_RECURSE PIEXAPP_SOURCES app/*.cpp)
    cmake_print_variables(PIEXAPP_SOURCES)

    add_executable(piexapp ${PIEXAPP_SOURCES})

    target_link_libraries(piexapp piexcore)
    target_link_libraries(piexapp piexbasic)
    target_link_libraries(piexapp piexsdl)
    target_include_directories(piexapp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif(ENABLE_SDL)

add_subdirectory(tests)
