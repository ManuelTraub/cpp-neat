cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

PROJECT(NEAT)

add_definitions(-D__DEBUG__)
add_subdirectory("./src")

enable_testing()
add_subdirectory("./test")
