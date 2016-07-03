/**
 * Class enscapsulating Genomes representating different Organisms within the same species
 */
#ifndef __SPECIES_H__
#define __SPECIES_H__

#include "Organism.h"
#include <memory>

using std::shared_ptr;

namespace NEAT {
    
    class Species {
        
        private:

            /* threshold for young species to get a fitness boost */
            static unsigned youthThreshold;

            /* the fitness boost a young generation gets (in percent / 100) */
            static double youthBoost;

            /* threshold after which a stagnant species will get a fittnes penalty */
            static unsigned stagnantThreshold;

            /* the penalty for a stagnant species (in percent / 100) */
            static double stagnantPenalty;

            /* propability for a mutation only offspring */
            static double mutationOnlyPropability;

            /* threshold for species size to have the fittest Organism survive an epoch */
            static unsigned fittestSurvivalThreshold;

            /* the similarity threshold for Organism to be in the same species */
            static double similarityThreshold;

            /* global random value */
            static rand64_t *rand;

            /* The oldest Organism of this species 
             * which is used as base for similarity calculation */
            shared_ptr<Organism> oldest;

            /* the Organisms of this */
            vector<shared_ptr<Organism>> organisms;

            /* the best fittnes of this */
            double bestFittness;

            /* the generation this was born */
            uint64_t birthGeneration;

            /* the generation of the last fittnes improvment of this */
            uint64_t lastImproveGeneration;

            /* recalculates the oldes genome */
            void refresh();

            /* chose a Organisms for reproduction, propability based on the organisms fittnes */
            vector<unsigned> chooseOrganisms();

            /* creates a new offspring from the Organism with the given index */
            shared_ptr<Organism> createOffspring(unsigned index);

        public:
            
            /* creats a new species */
            Species() : oldest(nullptr), 
                        bestFittness(std::nan), 
                        birthGeneration(GenerationCounter::get()),
                        lastImproveGeneration(GenerationCounter::get()) { }

            /* get the number of Geneomes of this */
            unsigned size() const { return organisms.size(); }

            /* add a Organism at the end of this */
            void add(const shared_ptr<Organism> &o) { organisms.push_back(o); }

            /* calculate the simmilarity of the given Genotype
             * to the oldest genome of this species 
             */
            double similarity(const Organism &o) const { 
                assert(oldest != nullptr);
                return oldest->getGenome()->similarity(o->getGenome()); 
            }

            /* clalculate the fittness of all Organism */
            void calculateFittness(vector<vector<double>> inputs, vector<vector<double>> outputs);

            /* calculates the given number of offsprings for the next generation */
            vector<shared_ptr<Organism>> createOffsprings(unsigned nOffsprings)

            /* calculate an inter species offspring */
            shared_ptr<Organism> Species::createOffspring(Species &other) {

    };
}

#endif /* __SPECIES_H__ */
