#ifndef __TEST_ACCESS_H__
#define __TEST_ACCESS_H__

#include "../src/Population.h"
#include <utility>

namespace NEAT {

    struct TestAccess {

        static NeuronGene &addNeuron(Genotype &genotype, const ConnectionGene &gene) {
            return genotype.addNeuron(gene);
        }

        static ConnectionGene &addConnection(Genotype &genotype, const NeuronGene &input, const NeuronGene &output) {
            return genotype.addConnection(input, output);
        }

        static void advanceGeneration() {
            GenerationCounter::inc();
        }

        static void setFitness(Organism &organism, double fitness) {
            organism.setFitness(fitness);
        }

        static vector<shared_ptr<Species>> &species(Population &population) {
            return population.species;
        }

        static vector<shared_ptr<Organism>> prepareGenerationChange(Population &population) {
            return population.prepareGenerationChange();
        }

        static vector<unsigned> calculateOffspringsPerSpecies(Population &population, unsigned nOffsprings) {
            return population.calculateOffspringsPerSpecies(nOffsprings);
        }

        static vector<shared_ptr<Organism>> createInterSpeciesOffsprings(Population &population) {
            return population.createInterSpeciesOffsprings();
        }

        static vector<shared_ptr<Organism>> createInSpeciesOffsprings(Population &population, vector<unsigned> offspringsPerSpecies) {
            return population.createInSpeciesOffsprings(std::move(offspringsPerSpecies));
        }

        static void sortOffspringsIntoSpecies(Population &population, vector<shared_ptr<Organism>> offsprings) {
            population.sortOffspringsIntoSpecies(std::move(offsprings));
        }

        static void finishGenerationChange(Population &population, vector<shared_ptr<Organism>> lastGeneration) {
            population.finishGenerationChange(std::move(lastGeneration));
        }
    };
}

#endif /* __TEST_ACCESS_H__ */
