cmake_minimum_required(VERSION 2.8)

PROJECT(NEAT)

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

add_executable(GenotypeTest 
               GenotypeTest 
               ../src/Opts
               ../src/utils 
               ../src/Genotype 
               ../src/Gene 
               ../src/GeneHistory 
               ../src/GenerationCounter)

add_executable(PhenotypeTest 
               PhenotypeTest 
               ../src/Opts
               ../src/utils 
               ../src/Genotype 
               ../src/Phenotype 
               ../src/Gene 
               ../src/GeneHistory 
               ../src/GenerationCounter)

add_executable(GeneHistoryTest 
               GeneHistoryTest 
               ../src/GeneHistory 
               ../src/utils 
               ../src/Gene 
               ../src/GenerationCounter)

add_executable(OrganismTest
               OrganismTest
               ../src/Opts
               ../src/Organism
               ../src/utils 
               ../src/Genotype 
               ../src/Phenotype 
               ../src/Gene 
               ../src/GeneHistory 
               ../src/GenerationCounter)

add_executable(SpeciesTest
               SpeciesTest
               ../src/Species
               ../src/Opts
               ../src/Organism
               ../src/utils 
               ../src/Genotype 
               ../src/Phenotype 
               ../src/Gene 
               ../src/GeneHistory 
               ../src/GenerationCounter)

add_executable(PopulationTest
               PopulationTest
               ../src/Population
               ../src/Species
               ../src/Opts
               ../src/Organism
               ../src/utils 
               ../src/Genotype 
               ../src/Phenotype 
               ../src/Gene 
               ../src/GeneHistory 
               ../src/GenerationCounter)

add_executable(FourWinns
               FourWinns
               ../src/Opts
               ../src/utils
               ../src/Phenotype)

add_test(NAME Genotype            COMMAND GenotypeTest)
add_test(NAME Phenotype           COMMAND PhenotypeTest)
add_test(NAME GeneHistory         COMMAND GeneHistoryTest)
add_test(NAME Organism            COMMAND OrganismTest)
add_test(NAME Species             COMMAND SpeciesTest)
add_test(NAME Population          COMMAND PopulationTest)
