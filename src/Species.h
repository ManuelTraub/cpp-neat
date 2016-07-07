/**
 * Class enscapsulating Genomes representating different Organisms within the same species
 */
#ifndef __SPECIES_H__
#define __SPECIES_H__

#include <memory>
#include "utils.h"
#include "Genotype.h"
#include "Organism.h"

using std::shared_ptr;

namespace NEAT {

    class Species {
        
        private:

            /* global random value */
            static rand64_t *rand;

            /* The oldest Genome of this species 
             * which is used as base for similarity calculation */
            shared_ptr<Genotype> oldestGenome;

            /* the Organisms of this */
            vector<shared_ptr<Organism>> organisms;

            /* the best fittnes of this */
            double bestOriginalFitness;

            /* the average adjusted fitness of this species */
            double averageAdjustedFitness;

            /* the generation this was born */
            uint64_t birthGeneration;

            /* the generation of the last fittnes improvment of this */
            uint64_t lastImproveGeneration;

#ifdef __DEBUG__
        public:
#endif

            /* chose a Organisms for reproduction, propability based on the organisms fittnes */
            vector<unsigned> chooseOrganisms(unsigned nOffsprings);

            /* creates a new offspring from the Organism with the given index */
            shared_ptr<Organism> createOffspring(unsigned index);

        public:
            
            /* creats a new species */
            Species() : bestOriginalFitness(std::nan("Species")), 
                        averageAdjustedFitness(std::nan("Species")),
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
            double similarity(const shared_ptr<Organism> &o) const { 
                assert(this->oldestGenome != nullptr);
                return o->similarity(*this->oldestGenome);
            }

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

            /* returns the Organisms of this */
            vector<shared_ptr<Organism>> getOrganisms() const { return organisms; }

            /* returns the averageAdjustedFitness of this */
            double getAverageAdjustedFitness() const { return averageAdjustedFitness; }

            /* removes all individuals from this and resets a values */
            void reset() {
                this->organisms.clear();
                this->oldestGenome           = nullptr;
                this->bestOriginalFitness    = std::nan("Species");
                this->averageAdjustedFitness = std::nan("Species");
                this->birthGeneration        = GenerationCounter::get();
                this->lastImproveGeneration  = GenerationCounter::get();
            }

#ifdef __DEBUG__ 
            void setAverageAdjustedFitness(double fitness) { averageAdjustedFitness = fitness; }
#endif

    };

    /* compares two Species based on theyr best fittness */
    struct SpeciesComparator {
            bool operator () (const shared_ptr<Species> &a,
                              const shared_ptr<Species> &b) const {

                return a->getAverageAdjustedFitness() > b->getAverageAdjustedFitness();
            }
    };
}

#endif /* __SPECIES_H__ */
