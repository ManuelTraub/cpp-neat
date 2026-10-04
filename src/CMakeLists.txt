cmake_minimum_required(VERSION 2.8)

PROJECT(NEAVC)

if (${WIN32}) 
  SET (CMAKE_CXX_COMPILER           "/C/mingw64/bin/g++")
  SET (CMAKE_CXX_FLAGS              "-Wall -Wextra -g -std=c++14")

  SET (CMAKE_C_COMPILER             "/C/mingw64/bin/gc")
  SET (CMAKE_C_FLAGS                "-Wall -Wextra -g")
else()
  SET (CMAKE_CXX_COMPILER           "/usr/bin/g++")
  SET (CMAKE_CXX_FLAGS              "-Wall -Wextra -g -std=c++14")

  SET (CMAKE_C_COMPILER             "/usr/bin/gcc")
  SET (CMAKE_C_FLAGS                "-Wall -Wextra -g")
endif()

find_package(Threads REQUIRED)

add_library(neat STATIC
    AI/Gene.cpp
    AI/GeneHistory.cpp
    AI/GenerationCounter.cpp
    AI/Genotype.cpp
    AI/Phenotype.cpp
    AI/Organism.cpp
    AI/Species.cpp
    AI/Population.cpp
    AI/ParallelWorker.cpp
    utils/utils.cpp
    utils/Threads.cpp
)

target_include_directories(neat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(neat PUBLIC
    Threads::Threads
    IL
)
