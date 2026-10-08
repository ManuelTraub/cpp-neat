find_package(Threads REQUIRED)

add_library(neat STATIC
    Gene.cpp
    GeneHistory.cpp
    GenerationCounter.cpp
    Genotype.cpp
    Phenotype.cpp
    Organism.cpp
    Species.cpp
    Population.cpp
    ParallelWorker.cpp
    utils/utils.cpp
    utils/Threads.cpp
)

target_include_directories(neat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(neat PRIVATE -Wall -Wextra)
endif()

target_link_libraries(neat PUBLIC
    Threads::Threads
)
