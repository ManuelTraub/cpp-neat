/**
 * Abstract Genotype encoding a Individual within NEAT
 */
#ifndef __GENOTYPE_H__
#define __GENOTYPE_H__

#include <vector>
#include "utils.h"
#include "Gene.h"

using std::vector;
using std::pair;

namespace NEAT {
    
    class Genotype {
        
        private:

            /* global random value to mutate this */
            static rand128_t *rand;

            /* the number of layers of this */
            unsigned nLayers;
            
            /* the neurons of this */
            vector<NeuronGene *> neurons;

            /* the ConnectionGenes of this */
            vector<ConnectionGene *> connections;

        public:

            /* return the number of layers of this */
            unsigned getNumberLayers() { return nLayers; }

            /* return the neurons of this */
            vector<NeuronGene *> getNeurons() { return neurons; }

            /* returns the connections of this */
            vector<ConnectionGene *> getConnections() { return connections; }
            
            /* mutate this by inserting a new hidden neuron 
             * (returns the new created connections) */
            pair<ConnectionGene *, ConnectionGene *> mutateNeuron();

            /* mutate a given percentage of connections */
            void mutateConnections(double percent);
    };
}

#endif /* __GENOTYPE_H__ */
