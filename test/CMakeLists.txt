foreach(component Gene Genotype Phenotype GeneHistory Organism Species Population)
    add_executable(${component}Test ${component}Test.cpp)
    target_link_libraries(${component}Test PRIVATE neat)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${component}Test PRIVATE -Wall -Wextra -Wno-unused-result)
    endif()
endforeach()

function(add_neat_test name target)
    set(work_dir "${CMAKE_CURRENT_BINARY_DIR}/work/${name}")
    file(MAKE_DIRECTORY "${work_dir}")
    add_test(NAME ${name} COMMAND ${target} ${ARGN})
    set_tests_properties(${name} PROPERTIES
        WORKING_DIRECTORY "${work_dir}"
        ENVIRONMENT "NEAT_SEED=${NEAT_TEST_SEED}"
    )
endfunction()

foreach(component Gene Genotype Phenotype GeneHistory Organism Species)
    add_neat_test(${component} ${component}Test)
endforeach()

set(population_tests
    basicOrganismSpeedTest
    basicSpeciesSpeedTest
    advancedSpeciesSpeedTest
    basicGenerationChangeSpeedTest
    basicBinaryXORTest
    binaryXORQualityTest
    binaryXORZeroStartConectionsQualityTest
    basicRecurencyTest
    binaryXORQualityTanhTest
    binaryXORZeroStartConectionsQualityTanhTest
    finiteFitnessSelectionTest
    customFitnessDimensionsTest
    parallelEvaluationTest
)

foreach(test_name IN LISTS population_tests)
    add_neat_test(Population.${test_name} PopulationTest -t ${test_name})
endforeach()
