#include "Organism.h"
#include "Opts.h"

using namespace NEAVC;
using namespace AI;

#define fitterGenome(o1, o2) ((o1.fitness > o2.fitness) ? o1.genome : o2.genome)
#define weakerGenome(o1, o2) ((o1.fitness > o2.fitness) ? o2.genome : o1.genome)
#define biggerGenome(o1, o2)  ((o1.genome.getConnections().size() > o2.genome.getConnections().size()) ? o1.genome : o2.genome)
#define smallerGenome(o1, o2) ((o1.genome.getConnections().size() > o2.genome.getConnections().size()) ? o2.genome : o1.genome)


/* global random value */
rand64_t *Organism::rand = new_rand64_t(); 

/* create a new Organism for the given number of input and output neurons */
Organism::Organism(
    const unsigned inputs, 
    const unsigned outputs, 
    Opts &opts, 
    GeneHistory &geneHistory
) :
    opts(opts.freeze()),
    geneHistory(geneHistory),
    genome(inputs, outputs, opts, geneHistory), 
    fitness(std::nan("Organism")), 
    alive(true) { }

/* create a new Organism */
Organism::Organism(
    Opts &opts, 
    GeneHistory &geneHistory
) :
    opts(opts.freeze()),
    geneHistory(geneHistory),
    genome(opts, geneHistory), 
    fitness(std::nan("Organism")), 
    alive(true) { }

/* create a new Organism from the given genome */
Organism::Organism(
   Genotype &genome,
   Opts &opts, 
   GeneHistory &geneHistory
) : 
    opts(opts.freeze()),
    geneHistory(geneHistory),
    genome(genome),
    fitness(std::nan("Organism")), 
    alive(true) { }

/* creating a new Organism by cloning this */
Organism Organism::clone() {
    Organism newClone = *this;
    newClone.alive = true;

    return newClone;
}

/* creat a new Organism mixing the genes of the two given Organisms */
Organism::Organism(
    const Organism &parent1, 
    const Organism &parent2, 
    Opts &opts,
    GeneHistory &geneHistory
) : 
    opts(opts.freeze()),
    geneHistory(geneHistory),
    genome(
        (parent1.fitness == parent2.fitness) ? 
        Genotype(
            smallerGenome(parent1, parent2), 
            biggerGenome(parent1, parent2),
            opts,
            geneHistory
        ) :
        Genotype(
            fitterGenome(parent1, parent2), 
            weakerGenome(parent1, parent2),
            opts,
            geneHistory
        )
    ),
    fitness(std::nan("Organism")), 
    alive(true) { 

    assert(!std::isnan(parent1.fitness) && !std::isnan(parent2.fitness));
}

/* calculates the fitness of this */
void Organism::calculateFittness(vector<vector<double>> inputs, 
                                 vector<vector<double>> outputs) {

    /* calculate the original fitness of this */
    if (std::isnan(fitness) || opts.alwaysRecalculateFitness()) {
        Phenotype phenotype(genome, opts);
        this->fitness = 1.0 / (1.0 + phenotype.calculateError(inputs, outputs));
    }
}

/* mutates this */
void Organism::mutate() {

    this->fitness = std::nan("Organism");

    if (rand_range_d64(rand, 0, 1) < opts.addStructureMutationPropability()) 
        for(unsigned i = 0; i < 30; i++) 
            if (this->genome.mutateInsertStructure()) 
                return;

    if (rand_range_d64(rand, 0, 1) < opts.addNeuronMutationPropability()) 
        for(unsigned i = 0; i < 30; i++) 
            if (this->genome.mutateInsertNeuron()) 
                return;
    
    if (rand_range_d64(rand, 0, 1) < opts.addConnectionMutationPropability()) 
        for(unsigned i = 0; i < 100; i++) 
            if (this->genome.mutateInsertConnection())
                return;

    if (rand_range_d64(rand, 0, 1) < opts.removeStructureMutationPropability()) 
        for(unsigned i = 0; i < 30; i++) 
            if (this->genome.mutateDeleteStructure()) 
                return;

    if (rand_range_d64(rand, 0, 1) < opts.removeNeuronMutationPropability()) 
        for(unsigned i = 0; i < 30; i++) 
            if (this->genome.mutateDeleteNeuron()) 
                return;
    
    if (rand_range_d64(rand, 0, 1) < opts.removeConnectionMutationPropability()) 
        for(unsigned i = 0; i < 100; i++) 
            if (this->genome.mutateDeleteConnection())
                return;
    
    if (rand_range_d64(rand, 0, 1) < opts.mutateWeightsPropability())
        this->genome.mutateConnections();

    if (rand_range_d64(rand, 0, 1) < opts.mutateNeuronsPropability())
        this->genome.mutateNeurons();

    if (rand_range_d64(rand, 0, 1) < opts.mutateStructurePropability())
        this->genome.mutateStructures();
}

namespace NEAVC {

    namespace AI {

        /* compares two Organism based on theyr adjustedFitness fittness */
        bool orderOrganisms(const shared_ptr<Organism> &a, const shared_ptr<Organism> &b) {
            return a->getFitness() > b->getFitness();
        }
    }
}
