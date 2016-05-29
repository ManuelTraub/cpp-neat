cmake_minimum_required(VERSION 2.8)

PROJECT(NEAT)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D__DEBUG__)
endif()
add_subdirectory("./src")

enable_testing()
add_subdirectory("./test")
