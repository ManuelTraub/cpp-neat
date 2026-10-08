cmake_minimum_required(VERSION 3.16)

project(NEAT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NEAT_TEST_SEED "20261007" CACHE STRING "Process-wide RNG seed for CTest")

add_subdirectory("./src")

include(CTest)
if(BUILD_TESTING)
    add_subdirectory("./test")
endif()
