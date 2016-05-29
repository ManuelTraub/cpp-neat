cmake_minimum_required(VERSION 2.8)

PROJECT(NEAT)

if (${WIN32}) 
  SET (CMAKE_CXX_COMPILER           "/C/mingw64/bin/g++")
  SET (CMAKE_CXX_FLAGS              "-Wall -Wextra -g --std=c++14")

  SET (CMAKE_C_COMPILER             "/C/mingw64/bin/gc")
  SET (CMAKE_C_FLAGS                "-Wall -Wextra -g")
else()
  SET (CMAKE_CXX_COMPILER           "/usr/bin/g++")
  SET (CMAKE_CXX_FLAGS              "-Wall -Wextra -g --std=c++14")

  SET (CMAKE_C_COMPILER             "/usr/bin/gc")
  SET (CMAKE_C_FLAGS                "-Wall -Wextra -g")
endif()

add_executable(QuadTreeTest QuadTreeTest)
add_executable(HexTreeTest HexTreeTest)
add_test(NAME QuadTree COMMAND QuadTreeTest)
add_test(NAME HexTree COMMAND HexTreeTest)
