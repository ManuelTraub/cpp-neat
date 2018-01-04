cmake_minimum_required(VERSION 2.8)

PROJECT(NEAVC)

find_package(OpenGL REQUIRED)

add_subdirectory("./AI")

if (${WIN32}) 
  SET (CMAKE_CXX_COMPILER           "/C/mingw64/bin/g++-6")
  SET (CMAKE_CXX_FLAGS              "-Wall -Wextra -g -O3 -march=native --std=c++14")

  SET (CMAKE_C_COMPILER             "/C/mingw64/bin/gcc-6")
  SET (CMAKE_C_FLAGS                "-Wall -Wextra -g -O3 -march=native")
else()
  SET (CMAKE_CXX_COMPILER           "/usr/bin/g++-6")
  SET (CMAKE_CXX_FLAGS              "-Wall -Wextra -g -Wno-unused-result -O3 -march=native --std=c++14")

  SET (CMAKE_C_COMPILER             "/usr/bin/gcc-6")
  SET (CMAKE_C_FLAGS                "-Wall -Wextra -g -O3 -march=native")
endif()

