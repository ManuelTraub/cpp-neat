/**
 * A Individuum with a specific genome wich belongs to a
 * specific species and has a specific fittnes
 */
#ifndef __INDIVIDUUM_H__
#define __INDIVIDUUM_H__

#include "Genotype.h"
#include "Phenotype.h"
#include "Species.h"

namespace NEAT {

    class Individuum {

        private:

            /* the genome of this Individuum */
            Genotype &genome;

            /* the fitness of this individuum */
            double fitness;

            /* the species this individuum belongs to */
            Species *species;

        public:

            /* creates a new individuum from the given Genome */
            Individuum(const Genotype &genome) :
                genome(genome), fitness(std::nan), species(NULL) { }

            /* calculates the fitness of this */
            void calculateFittness(vector<vector<double>> inputs, 
                                   vector<vector<double>> outputs) {

                assert(species != NULL);

                if (std::isnan(fitness)) {
                    const Phenotype phenotype(genome);
                    const double squareError = phenotype.calculateError(inputs, outputs);
                    this->fitness = squareError / this->species.size();
                }
            }

            /* returns a reference to the genome of this */
            Genotype &getGenome() const { return genome; }

            /* returns the fitness of this */
            double getFitness() const { 
                assert(!std::isnan(fitness));
                return fitness; 
            }

            /* returns the Species of this */
            Species &getSpecies() const { 
                assert(species != NULL);
                return *species; 
            }

            /* sets the species of this */
            void setSpecies(const Species &species) { species = &species; }

            /* compares this with other based on the fitness of this */
            bool operator < (const Individuum &other) const {
                assert(!std::isnan(fitness) && !std::isnan(other.fitness));
                return fitness < other.fitness;
            }
    };

}

#endif /* __INDIVIDUUM_H__ */
