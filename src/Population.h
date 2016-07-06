/**
 * encapsulation of the population of NEAT which consists out of several species
 */
#ifndef __POPULATION_H__
#define __POPULATION_H__

#include "Species.h"
#include <memory>

using std::shared_ptr;
using std::vector;

namespace NEAT {
    
    class Population {

        private:

            /* the species of this */
            vector<shared_ptr<Species>> species;

            /* the training data set */
            vector<vector<double>> inputs;
            vector<vector<double>> outputs;

            /* global random value */
            static rand64_t *rand;

            /* sorts the Organisms and species as removing the deaths 
             * returns the organisms left for reproduction 
             */
            vector<shared_ptr<Organism>> prepareGenerationChange();

            /* calculate the ofsprings per species */
            vector<unsigned> calculateOffspringsPerSpecies(unsigned nOffsprings);

            /* calculate the interspecies offsprings */
            vector<shared_ptr<Organism>> calculateInterSpeciesOffsprings();

        public:

            /* create a new Population */
            Population(vector<vector<double>> inputs, vector<vector<double>> outputs);

            /* increase the genaration counter and sort all genomes by theyr fitness */
            void calcNextGeneration();
    };
}


#endif /* __POPULATION_H__ */
