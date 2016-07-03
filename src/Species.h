/**
 * Class enscapsulating Genomes representating different Organisms within the same species
 */
#ifndef __SPECIES_H__
#define __SPECIES_H__

#include <memory>
#include "utils.h"
#include "Genotype.h"

using std::shared_ptr;

namespace NEAT {

    /* forward declaration */
    class Organism;
    
    class Species {
        
        private:

            /* global random value */
            static rand64_t *rand;

            /* The oldest Genome of this species 
             * which is used as base for similarity calculation */
            Genotype oldestGenome;

            /* the Organisms of this */
            vector<shared_ptr<Organism>> organisms;

            /* the best fittnes of this */
            double bestFittness;

            /* the generation this was born */
            uint64_t birthGeneration;

            /* the generation of the last fittnes improvment of this */
            uint64_t lastImproveGeneration;

            /* chose a Organisms for reproduction, propability based on the organisms fittnes */
            vector<unsigned> chooseOrganisms(unsigned nOffsprings);

            /* creates a new offspring from the Organism with the given index */
            shared_ptr<Organism> createOffspring(unsigned index);

        public:
            
            /* creats a new species */
            Species() : bestFittness(std::nan("Species")), 
                        birthGeneration(GenerationCounter::get()),
                        lastImproveGeneration(GenerationCounter::get()) { }

            /* get the number of Geneomes of this */
            unsigned size() const { return organisms.size(); }

            /* add a Organism at the end of this */
            void add(const shared_ptr<Organism> &o) { organisms.push_back(o); }

            /* remove all dead Organisms */
            void removeDeaths();

            /* calculates the new oldest genome */
            void calculateOldesGenome();

            /* calculate the simmilarity of the given Genotype
             * to the oldest genome of this species 
             */
            double similarity(shared_ptr<Organism> &o) const;

            /* clalculate the fittness of all Organism */
            void calculateFittness(vector<vector<double>> inputs, vector<vector<double>> outputs);

            /* calculates the given number of offsprings for the next generation */
            vector<shared_ptr<Organism>> createOffsprings(unsigned nOffsprings);

            /* calculate an inter species offspring */
            shared_ptr<Organism> createOffspring(Species &other);

            /* returns the age of this */
            uint64_t age() const { return GenerationCounter::get() - birthGeneration; }

            /* returns the stagnant age */
            uint64_t stagnantAge() const { 
                return GenerationCounter::get() - lastImproveGeneration; 
            }

            /* compares this with other based on the best fitness of this */
            bool operator < (const Species &other) const {
                return bestFittness < other.bestFittness;
            }

            /* returns the Organisms of this */
            vector<shared_ptr<Organism>> getOrganisms() const { return organisms; }

    };
}

#endif /* __SPECIES_H__ */
