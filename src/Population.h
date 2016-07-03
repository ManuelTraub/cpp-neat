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

            /* the survival rate of this poppulation during generation change */
            double survivalRate;

            /* the similarity threshold for individuals to be in the same species */
            double similarityThreshold;

            /* propability of interspecies mating */
            double interSpeciesMatingPropability;

            /* desired number of Organism per generation */
            unsigned populationSize;

            /* the training data set */
            vector<vector<double>> inputs;
            vector<vector<double>> outputs;

            /* global random value */
            static rand64_t *rand;

        public:

            /* increase the genaration counter and sort all genomes by theyr fitness */
            void calcNextGeneration();
    };
}


#endif /* __POPULATION_H__ */
