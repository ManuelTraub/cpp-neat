/**
 * A Organism with a specific genome wich belongs to a
 * specific species and has a specific fittnes
 */
#ifndef __INDIVIDUUM_H__
#define __INDIVIDUUM_H__

#include "Genotype.h"
#include "Phenotype.h"
#include <memory>

using std::shared_ptr;

namespace NEAT {

    class Organism {

        private:

            /* global random value */
            static rand64_t *rand;

            /* the genome of this Organism */
            Genotype genome;

            /* the original fitness of this Organism */
            double originalFitness;

            /* the adjusted fitness of this Organism */
            double adjustedFitness;

            /* indicates that this Organism is alive */
            bool alive;

            /* mutates this */
            void mutate();

        public:

            /* create a new Organism for the given number of input and output neurons */
            Organism(const unsigned inputs, const unsigned outputs) :
                genome(Genotype(inputs, outputs)), 
                originalFitness(std::nan("Organism")), 
                adjustedFitness(std::nan("Organism")), 
                alive(true) { }

            /* creating a new Organism by mutating the given Organism */
            Organism(const Organism &parent) :
                genome(parent.genome),
                originalFitness(std::nan("Organism")), 
                adjustedFitness(std::nan("Organism")), 
                alive(true) {
                
                this->mutate();
            }

            /* creat a new Organism mixing the genes of the two given Organisms */
            Organism(const Organism &parent1, const Organism &parent2) : 
                genome((parent1.originalFitness < parent2.originalFitness) ? 
                        Genotype(parent1.genome, parent2.genome)           :
                        Genotype(parent2.genome, parent1.genome)),
                originalFitness(std::nan("Organism")), 
                adjustedFitness(std::nan("Organism")), 
                alive(true) { 
                
                assert(!std::isnan(parent1.originalFitness) &&
                       !std::isnan(parent2.originalFitness) &&
                       !std::isnan(parent1.adjustedFitness) &&
                       !std::isnan(parent2.adjustedFitness));
            }

            /* calculates the fitness of this */
            void calculateFittness(vector<vector<double>> inputs, 
                                   vector<vector<double>> outputs,
                                   unsigned speciesSize,
                                   unsigned speciesAge,
                                   unsigned speciesStagnantAge);


            /* returns the original fitness of this */
            double getOriginalFitness() const { 
                assert(!std::isnan(originalFitness));
                return originalFitness; 
            }

            /* returns the adjusted fitness of this */
            double getAdjustedFitness() const { 
                assert(!std::isnan(adjustedFitness));
                return adjustedFitness; 
            }

            /* returns whether this Organism is alive */
            bool isAlive() const { return alive; }

            /* kills this Organism */
            void kill() { 
                assert(alive);
                alive = false;
            }

            /* returns the similarity with this and the given genome */
            double similarity(const Genotype &genome) const {
                return this->genome.similarity(genome);
            }

            /* returns a copy of the Genome of this */
            Genotype getGenome() const { return genome; }

#ifdef __DEBUG__
            void setOriginalFitness(double fitness) { originalFitness = fitness; }
            void setAdjustedFitness(double fitness) { adjustedFitness = fitness; }
#endif

    };

    /* compares two Organism based on theyr adjustedFitness fittness */
    struct OrganismComparator {
            bool operator () (const shared_ptr<Organism> &a,
                              const shared_ptr<Organism> &b) const {

                return a->getAdjustedFitness() > b->getAdjustedFitness();
            }
    };

}

#endif /* __INDIVIDUUM_H__ */
