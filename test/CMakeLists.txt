cmake_minimum_required(VERSION 2.8)

PROJECT(NEAVC)

find_package(OpenGL REQUIRED)

add_subdirectory("./AI")
add_subdirectory("./Physics")
add_subdirectory("./Graphics")

if (${WIN32}) 
  SET (CMAKE_CXX_COMPILER           "/C/mingw64/bin/g++")
  SET (CMAKE_CXX_FLAGS              "-Wall -Wextra -g -O3 -march=native --std=c++14")

  SET (CMAKE_C_COMPILER             "/C/mingw64/bin/gc")
  SET (CMAKE_C_FLAGS                "-Wall -Wextra -g -O3 -march=native")
else()
  SET (CMAKE_CXX_COMPILER           "/usr/bin/g++")
  SET (CMAKE_CXX_FLAGS              "-Wall -Wextra -g -Wno-unused-result -O3 -march=native --std=c++14")

  SET (CMAKE_C_COMPILER             "/usr/bin/gcc")
  SET (CMAKE_C_FLAGS                "-Wall -Wextra -g -O3 -march=native")
endif()

add_executable(VirtualCreatureTest
               VirtualCreatureTest
               ../src/VESimulation
               ../src/VirtualCreature
               ../src/AI/Gene
               ../src/AI/GeneHistory
               ../src/AI/GenerationCounter
               ../src/AI/Genotype
               ../src/AI/Phenotype
               ../src/Physics/3DBase
               ../src/Physics/MassSpring
               ../src/Physics/TOSoftBody
               ../src/Physics/PhysicsSimulation
               ../src/Physics/RealtimePhysics
               ../src/Graphics/GLWindow
               ../src/Graphics/GLObject
               ../src/Graphics/GLOrbitingCamera
               ../src/utils/Threads
               ../src/Graphics/GPConverter
               ../src/Graphics/loadShaders
               ../src/utils/utils)

add_executable(VEPopulationTest
               VEPopulation
               ../src/VESimulation
               ../src/VirtualCreature
               ../src/AI/Gene
               ../src/AI/GeneHistory
               ../src/AI/GenerationCounter
               ../src/AI/Genotype
               ../src/AI/Phenotype
               ../src/AI/Organism
               ../src/AI/Species
               ../src/AI/Population
               ../src/AI/ParallelWorker
               ../src/Physics/3DBase
               ../src/Physics/MassSpring
               ../src/Physics/TOSoftBody
               ../src/Physics/PhysicsSimulation
               ../src/Physics/RealtimePhysics
               ../src/Graphics/GLWindow
               ../src/Graphics/GLObject
               ../src/Graphics/GLOrbitingCamera
               ../src/utils/Threads
               ../src/Graphics/GPConverter
               ../src/Graphics/loadShaders
               ../src/utils/utils)

target_link_libraries(VirtualCreatureTest
    ${OPENGL_LIBRARY}
    glfw
    GLEW
    pthread
)

target_link_libraries(VEPopulationTest
    ${OPENGL_LIBRARY}
    glfw
    GLEW
    pthread
)

add_test(NAME VirtualCreatureTest COMMAND VirtualCreatureTest)
add_test(NAME VEPopulationTest COMMAND VEPopulationTest)
