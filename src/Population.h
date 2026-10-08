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

        /* foreard declarations */
        struct FitnessCalculator;

        /* abstract class to access the species of this during training */
        struct SpeciesInspector {

            /* gets called just bevor the fitness calculation
             * (can be used for fitness calculation based on whole Population)
             * Note: species are not sorted
             */
            virtual void bevor(vector<shared_ptr<Species>>) { }

            /* gets called right after fittnes calculation
             * Note: species are not sorted
             */
            virtual void during(vector<shared_ptr<Species>>) { }

            /* called after selection 
             * (after not surviving Organisms are killed and species are sorted) 
             */
             virtual void after(vector<shared_ptr<Species>>) { }

            /* abstract destructor */
            virtual ~SpeciesInspector() = 0;

        };
        
        class Population {

            private:

                friend struct TestAccess;

                /* the Options of this */
                Opts &opts;

                /* the GeneHistory of this */
                GeneHistory geneHistory;

                /* the species of this */
                vector<shared_ptr<Species>> species;

                /* the training data set */
                vector<vector<double>> inputs;
                vector<vector<double>> outputs;

                /* global random value */
                static rand64_t *rand;

                /* rand function for shuffling the training data */
                static int shuffleRand(int i) { return rand64(rand) % i; }

                /* the FitnessCalculator of this */
                FitnessCalculator *fitnessCalculator;

                /* the SpeciesInspectors of this */
                vector<SpeciesInspector *> speciesInspectors;

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
                Population(
                    vector<vector<double>> inputs, 
                    vector<vector<double>> outputs, 
                    Opts &opts,
                    FitnessCalculator *fitnessCalculator = NULL
                );

                /* create a new Population */
                Population(FitnessCalculator *fitnessCalculator, Opts &opts);

                /* increase the genaration counter and sort all genomes by theyr fitness */
                void calcNextGeneration();

                /* returns the best Organism */
                shared_ptr<Organism> getBestOrganism();

                /* create all initial connections */
                void createInitialConnections(bool connectBias = true);

                /* adds a SpeciesInspector for this */
                void add(SpeciesInspector *inspector) {
                    this->speciesInspectors.push_back(inspector);
                }

        };
}


#endif /* __POPULATION_H__ */
