#include "Organism.h"
#include "Opts.h"

using namespace NEAT;

/* global random value */
rand64_t *Organism::rand = new_rand64_t(); 


/* calculates the fitness of this */
void Organism::calculateFittness(vector<vector<double>> inputs, 
                                 vector<vector<double>> outputs,
                                 unsigned speciesSize,
                                 unsigned speciesAge,
                                 unsigned speciesStagnantAge) {

    /* calculate the original fitness of this */
    if (std::isnan(originalFitness)) {
        Phenotype phenotype(genome);
        this->originalFitness = 1.0 / phenotype.calculateError(inputs, outputs);
    }

    /* adjust fitness according to the size of the Species */
    this->adjustedFitness = this->originalFitness;

    /* adjust fitness according to the age of the species 
     * younger species will get a fitness boost
     */
    if (speciesAge < Opts::youthThreshold())
        this->adjustedFitness *= Opts::youthBoost();

    /* adjust fitness of stagnant species */
    if (speciesStagnantAge > Opts::stagnantThreshold())
        this->adjustedFitness *= Opts::stagnantPenalty();
}

/* penalizes the fitness of this Organism */
void Organism::penalize() {
    assert(!std::isnan(this->adjustedFitness));
    this->adjustedFitness *= Opts::stagnantPenalty();
}

/* mutates this */
void Organism::mutate() {

    this->originalFitness = std::nan("Organism");
    this->adjustedFitness = std::nan("Organism");

    double boost = 1.0;

//    if (GenerationCounter::get() < Opts::structuralMutationThreshold())
//        boost = Opts::structuralMutationBoost();

    if (rand_range_d64(rand, 0, 1) < Opts::addNeuronMutationPropability() * boost) {
        for(unsigned i = 0; i < 20; i++)
            if (this->genome.mutateInsertNeuron())
                return;
    } 
    
    if (rand_range_d64(rand, 0, 1) < Opts::addConnectionMutationPropability() * boost) {
        for(unsigned i = 0; i < 20; i++)
            if (this->genome.mutateInsertConnection())
                return;
    }
    
    if (rand_range_d64(rand, 0, 1) < Opts::mutateWeightsPropability())
        this->genome.mutateConnections();
}
