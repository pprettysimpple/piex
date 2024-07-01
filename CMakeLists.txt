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

# headers from libraries, needed for build
set(PIEX_EXTERNAL_HEADERS_DIR ${CMAKE_SOURCE_DIR}/deps/include)

option(ENABLE_SDL "ENABLE_SDL" OFF)
if(ENABLE_SDL)
    message(STATUS "Building with SDL2")
    # static lib piexsdl
    find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2)


    file(GLOB_RECURSE PIEX_SDL_SOURCES impl_sdl/*.cpp)
    cmake_print_variables(PIEX_SDL_SOURCES)

    add_library(piexsdl STATIC ${PIEX_SDL_SOURCES})

    target_link_libraries(piexsdl PUBLIC SDL2::SDL2)
    target_link_libraries(piexsdl PUBLIC piexcore)
    target_include_directories(piexsdl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    # workaround, because sdl release comes with paths like SDL2/include/SDL_shit.h
    # and we can not use standard paths like SDL2/SDL_shit.h
    file(GLOB PIEX_SDL2_HEADERS "${SDL2_INCLUDE_DIRS}/*.h")
    file(COPY ${PIEX_SDL2_HEADERS} DESTINATION ${PIEX_EXTERNAL_HEADERS_DIR}/SDL2)
    file(COPY ${SDL2_DIR} DESTINATION ${CMAKE_BINARY_DIR})
    target_include_directories(piexsdl PUBLIC ${PIEX_EXTERNAL_HEADERS_DIR})

    # executable piexapp
    file(GLOB_RECURSE PIEXAPP_SOURCES app/*.cpp)
    cmake_print_variables(PIEXAPP_SOURCES)

    add_executable(piexapp ${PIEXAPP_SOURCES})

    target_link_libraries(piexapp piexbasic piexsdl)
    target_include_directories(piexapp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    if (WIN32)
        # copy the .dll file to the same folder with executable
        # needed for windows, if SDL is installed on custom path
        add_custom_command(
            TARGET piexapp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:piexapp>)
     endif()
endif(ENABLE_SDL)

add_subdirectory(tests)
