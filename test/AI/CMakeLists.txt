cmake_minimum_required(VERSION 2.8)

PROJECT(NEAVC)

if (${WIN32}) 
  SET (CMAKE_CXX_COMPILER           "/C/mingw64/bin/g++")
  SET (CMAKE_CXX_FLAGS              "-Wall -Wextra -g -O3 -march=native --std=c++14")

  SET (CMAKE_C_COMPILER             "/C/mingw64/bin/gcc")
  SET (CMAKE_C_FLAGS                "-Wall -Wextra -g -O3 -march=native")
else()
  SET (CMAKE_CXX_COMPILER           "/usr/bin/g++")
  SET (CMAKE_CXX_FLAGS              "-Wall -Wextra -g -Wno-unused-result -O3 -march=native --std=c++14")

  SET (CMAKE_C_COMPILER             "/usr/bin/gcc")
  SET (CMAKE_C_FLAGS                "-Wall -Wextra -g -O3 -march=native")
endif()

add_executable(GeneTest 
               GeneTest 
               ../../src/utils/utils 
               ../../src/AI/Gene
               ../../src/AI/GenerationCounter)

add_executable(GenotypeTest 
               GenotypeTest 
               ../../src/utils/utils 
               ../../src/AI/Genotype 
               ../../src/AI/Gene 
               ../../src/AI/GeneHistory 
               ../../src/AI/GenerationCounter)

add_executable(PhenotypeTest 
               PhenotypeTest 
               ../../src/utils/utils 
               ../../src/AI/Genotype 
               ../../src/AI/Phenotype 
               ../../src/AI/Gene 
               ../../src/AI/GeneHistory 
               ../../src/AI/GenerationCounter)

add_executable(GeneHistoryTest 
               GeneHistoryTest 
               ../../src/AI/GeneHistory 
               ../../src/utils/utils 
               ../../src/AI/Gene 
               ../../src/AI/GenerationCounter)

add_executable(OrganismTest
               OrganismTest
               ../../src/AI/Organism
               ../../src/utils/utils 
               ../../src/AI/Genotype 
               ../../src/AI/Phenotype 
               ../../src/AI/Gene 
               ../../src/AI/GeneHistory 
               ../../src/AI/GenerationCounter)

add_executable(SpeciesTest
               SpeciesTest
               ../../src/AI/Species
               ../../src/AI/Organism
               ../../src/utils/utils 
               ../../src/AI/Genotype 
               ../../src/AI/Phenotype 
               ../../src/AI/Gene 
               ../../src/AI/GeneHistory 
               ../../src/AI/GenerationCounter)

add_executable(PopulationTest
               PopulationTest
               ../../src/AI/Population
               ../../src/AI/ParallelWorker
               ../../src/AI/Species
               ../../src/AI/Organism
               ../../src/utils/utils 
               ../../src/AI/Genotype 
               ../../src/AI/Phenotype 
               ../../src/AI/Gene 
               ../../src/utils/Threads
               ../../src/AI/GeneHistory 
               ../../src/AI/GenerationCounter
               ../../src/utils/Threads)

add_executable(HexagonalEnvironmentTest
               HexagonalEnvironmentTest
               ../../src/HexagonalEnvironment
               ../../src/Physics/HexagonalPlayboard
               ../../src/Physics/3DBase
               ../../src/Physics/MassSpring
               ../../src/Physics/TOSoftBody
               ../../src/Physics/PhysicsSimulation
               ../../src/Physics/IcoSphere
               ../../src/Physics/RealtimePhysics
               ../../src/Physics/HexagonalPlayboard
               ../../src/Physics/PlayboardAgent
               ../../src/Graphics/GLWindow
               ../../src/Graphics/GLObject
               ../../src/Graphics/GL2DCamera
               ../../src/Graphics/GLPlayboardDesigner
               ../../src/Graphics/GLPlayboardAgentCamera
               ../../src/Graphics/GPConverter
               ../../src/Graphics/loadShaders
               ../../src/AI/Population
               ../../src/AI/ParallelWorker
               ../../src/AI/Species
               ../../src/AI/Organism
               ../../src/AI/Genotype 
               ../../src/AI/Phenotype 
               ../../src/AI/Gene 
               ../../src/AI/GeneHistory 
               ../../src/AI/GenerationCounter
               ../../src/utils/Threads
               ../../src/utils/utils)

add_executable(HexagonalExpirienceLearner
               HexagonalExpirienceLearner
               ../../src/HexagonalEnvironment
               ../../src/Physics/HexagonalPlayboard
               ../../src/Physics/3DBase
               ../../src/Physics/MassSpring
               ../../src/Physics/TOSoftBody
               ../../src/Physics/PhysicsSimulation
               ../../src/Physics/IcoSphere
               ../../src/Physics/RealtimePhysics
               ../../src/Physics/HexagonalPlayboard
               ../../src/Physics/PlayboardAgent
               ../../src/Graphics/GLWindow
               ../../src/Graphics/GLObject
               ../../src/Graphics/GL2DCamera
               ../../src/Graphics/GLPlayboardDesigner
               ../../src/Graphics/GLPlayboardAgentCamera
               ../../src/Graphics/GPConverter
               ../../src/Graphics/loadShaders
               ../../src/utils/Threads
               ../../src/utils/utils)

target_link_libraries(HexagonalExpirienceLearner
    ${OPENGL_LIBRARY}
    glfw
    GLEW
    pthread
    png
    IL
)

target_link_libraries(HexagonalEnvironmentTest
    ${OPENGL_LIBRARY}
    glfw
    GLEW
    pthread
    png
    IL
)

target_link_libraries(PopulationTest -pthread IL)
target_link_libraries(GeneTest IL)
target_link_libraries(GenotypeTest IL)
target_link_libraries(PhenotypeTest IL)
target_link_libraries(GeneHistoryTest IL)
target_link_libraries(OrganismTest IL)
target_link_libraries(SpeciesTest IL)

add_test(NAME Gene                 COMMAND GeneTest)
add_test(NAME Genotype             COMMAND GenotypeTest)
add_test(NAME Phenotype            COMMAND PhenotypeTest)
add_test(NAME GeneHistory          COMMAND GeneHistoryTest)
add_test(NAME Organism             COMMAND OrganismTest)
add_test(NAME Species              COMMAND SpeciesTest)
add_test(NAME Population           COMMAND PopulationTest)
add_test(NAME HexagonalEnvironment COMMAND HexagonalEnvironmentTest)
add_test(NAME HexagonalExpirienceLearnerTest COMMAND HexagonalExpirienceLearner)
