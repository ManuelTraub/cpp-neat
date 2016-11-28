/**
 * encapsulation of the population of NEAT which consists out of several species
 */
#ifndef __POPULATION_H__
#define __POPULATION_H__

#include "Species.h"
#include "Opts.h"
#include <memory>

using std::shared_ptr;
using std::vector;

namespace NEAT {
    
    class Population {

        private:

            /* the Options of this */
            Opts &opts;

            /* the species of this */
            vector<shared_ptr<Species>> species;

            /* the training data set */
            vector<vector<double>> inputs;
            vector<vector<double>> outputs;

            /* global random value */
            static rand64_t *rand;

            /* rand function for shuffling the training data */
            static int shuffleRand(int i) { return rand64(rand) % i; }

            /* the best overal (original) fittnes of this */
            double bestFitness;

            /* the generation of the last fittnes improvment of this */
            uint64_t lastImproveGeneration;

#ifdef __DEBUG__
        public:
#endif

            /* sorts the Organisms and species as removing the deaths 
             * returns the organisms left for reproduction 
             */
            vector<shared_ptr<Organism>> prepareGenerationChange();

            /* calculate the ofsprings per species */
            vector<unsigned> calculateOffspringsPerSpecies(unsigned nOffsprings);

            /* calculate the interspecies offsprings */
            vector<shared_ptr<Organism>> createInterSpeciesOffsprings();

            /* clalculates the offsprings for each species 
             * returns the offsprings which arent similar to the species they came from
             */
            vector<shared_ptr<Organism>> 
            createInSpeciesOffsprings(vector<unsigned> offspringsPerSpecies);

            /* sort the given offsprings into species (create new species if neccesarry) */
            void sortOffspringsIntoSpecies(vector<shared_ptr<Organism>> offsprings);

            /* finish the generation chane by cleaning up the death individuals 
             * and increasing the GenerationCounter
             */
            void finishGenerationChange(vector<shared_ptr<Organism>> lastGeneration);

        public:

            /* create a new Population */
            Population(vector<vector<double>> inputs, 
                       vector<vector<double>> outputs, 
                       Opts &opts);

            /* increase the genaration counter and sort all genomes by theyr fitness */
            void calcNextGeneration();

            /* returns the best Organism */
            shared_ptr<Organism> getBestOrganism();

            /* create all initial connections */
            void createInitialConnections(bool connectBias = true);

#ifdef __DEBUG__
            vector<shared_ptr<Species>> &getSpecies() { return this->species; }
#endif 
    };
}


#endif /* __POPULATION_H__ */
