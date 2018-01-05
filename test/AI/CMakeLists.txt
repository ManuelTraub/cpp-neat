cmake_minimum_required(VERSION 2.8)

PROJECT(NEAVC)

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

target_link_libraries(PopulationTest -pthread)

add_test(NAME Gene                COMMAND GeneTest)
add_test(NAME Genotype            COMMAND GenotypeTest)
add_test(NAME Phenotype           COMMAND PhenotypeTest)
add_test(NAME GeneHistory         COMMAND GeneHistoryTest)
add_test(NAME Organism            COMMAND OrganismTest)
add_test(NAME Species             COMMAND SpeciesTest)
add_test(NAME Population          COMMAND PopulationTest)
