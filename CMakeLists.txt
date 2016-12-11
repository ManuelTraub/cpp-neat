cmake_minimum_required(VERSION 2.8)

PROJECT(NEAVC)

add_subdirectory("./src")

enable_testing()
add_definitions(-D__DEBUG__)
add_subdirectory("./test")
