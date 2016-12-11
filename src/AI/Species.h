/**
 * Class enscapsulating Genomes representating different Organisms within the same species
 */
#ifndef __SPECIES_H__
#define __SPECIES_H__

#include <memory>
#include "utils.h"
#include "Genotype.h"
#include "Organism.h"
#include "Opts.h"

using std::shared_ptr;

namespace NEAVC {

    namespace AI {

        class Species {
            
            private:

                /* reference to the options of the whole population */
                Opts &opts;

                /* global random value */
                static rand64_t *rand;

                /* the Organisms of this */
                vector<shared_ptr<Organism>> organisms;

                /* the best fittnes of this */
                double bestFitness;

                /* the average adjusted fitness of this species */
                double averageFitness;

                /* the generation this was born */
                uint64_t birthGeneration;

                /* the generation of the last fittnes improvment of this */
                uint64_t lastImproveGeneration;

#ifdef __DEBUG__
            public:
#endif

                /* creates a new offspring from the Organism with the given index */
                shared_ptr<Organism> createOffspring(unsigned index);

            public:
                
                /* creats a new species */
                Species(Opts &opts) : opts(opts.freeze()),
                                      bestFitness(std::nan("Species")), 
                                      averageFitness(std::nan("Species")),
                                      birthGeneration(GenerationCounter::get()),
                                      lastImproveGeneration(GenerationCounter::get()) { }

                /* get the number of Geneomes of this */
                unsigned size() const { return organisms.size(); }

                /* add a Organism at the end of this */
                void add(const shared_ptr<Organism> &o) { organisms.push_back(o); }

                /* remove all dead Organisms */
                void removeDeaths();

                /* calculate the simmilarity of the given Genotype
                 * to the fittest genome of this
                 */
                double similarity(const shared_ptr<Organism> &o) const { 
                    return o->similarity(*this->organisms[0]);
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

                /* returns the averageFitness of this */
                double getAverageFitness() const { return averageFitness; }

                /* removes all individuals from this and resets a values */
                void reset() {
                    this->organisms.clear();
                    this->bestFitness            = std::nan("Species");
                    this->averageFitness = std::nan("Species");
                    this->birthGeneration        = GenerationCounter::get();
                    this->lastImproveGeneration  = GenerationCounter::get();
                }

                /* kills the worst performing Organisms 
                 * (keeps at least one Organism) 
                 */
                void killWorst();

                /* returns the best original fittness of this */
                double getBestFitness() const { return bestFitness; }

#ifdef __DEBUG__ 
                void setAverageFitness(double fitness) { averageFitness = fitness; }
#endif

        };

        /* compares two Species based on theyr best fittness */
        bool orderSpecies(const shared_ptr<Species> &a, const shared_ptr<Species> &b);

    }
}

#endif /* __SPECIES_H__ */
