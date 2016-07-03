/**
 * A Organism with a specific genome wich belongs to a
 * specific species and has a specific fittnes
 */
#ifndef __INDIVIDUUM_H__
#define __INDIVIDUUM_H__

#include "Genotype.h"
#include "Phenotype.h"

namespace NEAT {

    /* forward declaration */
    class Species;

    class Organism {

        private:

            /* the genome of this Organism */
            Genotype genome;

            /* the fitness of this individuum */
            double fitness;

            /* indicates that this Organism is alive */
            bool alive;

        public:

            /* creates a new individuum from the given Genome */
            Organism(const Genotype &genome) :
                genome(genome), fitness(std::nan("Organism")), alive(true) { }

            /* calculates the fitness of this */
            void calculateFittness(vector<vector<double>> inputs, 
                                   vector<vector<double>> outputs,
                                   Species &species);


            /* returns a reference to the genome of this */
            Genotype &getGenome() { return genome; }

            /* returns the fitness of this */
            double getFitness() const { 
                assert(!std::isnan(fitness));
                return fitness; 
            }

            /* compares this with other based on the fitness of this */
            bool operator < (const Organism &other) const {
                assert(!std::isnan(fitness) && !std::isnan(other.fitness));
                return fitness < other.fitness;
            }

            /* returns whether this Organism is alive */
            bool isAlive() const { return alive; }

            /* kills this Organism */
            void kill() { 
                assert(alive);
                alive = false;
            }

    };

}

#endif /* __INDIVIDUUM_H__ */
