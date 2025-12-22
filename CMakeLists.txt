cmake_minimum_required(VERSION 4.0.0)
project(advent_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
add_compile_options(
    -Wall
    -Wshadow
    -Wcast-align
    -Wpointer-arith
    -Wwrite-strings
    -Wunused
    -Wunused-function
    -Wunused-variable
    -Wunused-parameter
    -Wunused-but-set-variable
    -Wunused-but-set-parameter
)


# Aggressive optimizations for Release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-march=native -mtune=native -flto=auto -ffast-math -funroll-loops)
    add_link_options(-flto=auto)
endif()

file(GLOB_RECURSE DAY_SOURCES "day*/day*.cpp")

foreach(SOURCE_FILE ${DAY_SOURCES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE_FILE} NAME_WE)

    get_filename_component(SOURCE_DIR ${SOURCE_FILE} DIRECTORY)
    get_filename_component(DAY_DIR ${SOURCE_DIR} NAME)

    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE})

    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${DAY_DIR}")
endforeach()
