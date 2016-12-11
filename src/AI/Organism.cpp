#include "Organism.h"
#include "Opts.h"

using namespace NEAVC;
using namespace AI;

/* global random value */
rand64_t *Organism::rand = new_rand64_t(); 


/* calculates the fitness of this */
void Organism::calculateFittness(vector<vector<double>> inputs, 
                                 vector<vector<double>> outputs) {

    /* calculate the original fitness of this */
    if (std::isnan(fitness)) {
        Phenotype phenotype(genome, opts);
        this->fitness = 1.0 / phenotype.calculateError(inputs, outputs);
    }
}

/* mutates this */
void Organism::mutate() {

    this->fitness = std::nan("Organism");

    if (rand_range_d64(rand, 0, 1) < opts.addNeuronMutationPropability()) 
        for(unsigned i = 0; i < 30; i++) 
            if (this->genome.mutateInsertNeuron()) 
                return;
    
    if (rand_range_d64(rand, 0, 1) < opts.addConnectionMutationPropability()) 
        for(unsigned i = 0; i < 100; i++) 
            if (this->genome.mutateInsertConnection())
                return;
    
    if (rand_range_d64(rand, 0, 1) < opts.mutateWeightsPropability())
        this->genome.mutateConnections();

    if (rand_range_d64(rand, 0, 1) < opts.mutateGradientPropability())
        this->genome.mutateNeurons();
}

namespace NEAVC {

    namespace AI {

        /* compares two Organism based on theyr adjustedFitness fittness */
        bool orderOrganisms(const shared_ptr<Organism> &a, const shared_ptr<Organism> &b) {
            return a->getFitness() > b->getFitness();
        }
    }
}
