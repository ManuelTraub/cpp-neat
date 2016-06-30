#include "Population.h"

using namespace NEAT;

/* increase the genaration counter and sort all genomes by theyr fitness */
void Population::calcNextGeneration() {
    
    assert(genomes.size() == genomeSpecies.size() && genome.size() == fitnes.size());

    /* calculate the fitness of all genomes */
    for (auto &iv : ivs)
        iv.calculateFittness(inputs, outputs);

    /* stort the individuals by theyr fitness */
    std::sort(ivs.begin(), ivs.end());


    // TODO kill recombinate and mutate ivs
    

    /* resort the individuals into species */
    for (auto &s : species)
        s.clear();

    for (auto &iv : ivs) {

        bool createNewSpecies = true;
        for (auto &s : species) {
            if (s.similarity(iv) < similarityThreshold) {
                s.add(iv.getGenome());
                iv.setSpecies(s);
                createNewSpecies = false;
                break;
            }
        }
        
        /* create a new species if the individuum does not belong to any species */
        if (createNewSpecies) {
            species.push_back(Species());
            species.back().add(iv.getGenome());
            species.back().refresh();
            iv.setSpecies(species.back());
        }
    }

    /* recalculate the oldest individuum for all species */
    for (auto &s : species)
        s.refresh();

}
