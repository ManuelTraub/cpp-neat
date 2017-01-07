cmake_minimum_required(VERSION 2.8)

PROJECT(NEAVC)

find_package(OpenGL REQUIRED)

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

add_executable(neavc
    main
    Graphics/loadShaders
    utils/utils
    Graphics/GLWindow
    Graphics/GLObject
    Graphics/GLOrbitingCamera
    Graphics/GPConverter
    Physics/3DBase
    Physics/MassSpring
    Physics/PhysicsSimulation
    Physics/RealtimePhysics
    utils/Threads
)

set(ALL_LIBS
    ${OPENGL_LIBRARY}
    glfw
    GLEW
    pthread
)

target_link_libraries(neavc
    ${ALL_LIBS}
)
