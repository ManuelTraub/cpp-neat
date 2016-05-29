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
            static rand64_t *rand;

            /* the HexTree storing all created Connections */
            static ConnectionTree globalConnections;

            /* global lock */
            static pthread_mutex_t mutex;

            /* the neurons of this */
            vector<NeuronGene *> neurons;

            /* fast access to the neurons of this */
            NeuronTree neuronsTree;

            /* the ConnectionGenes of this */
            vector<ConnectionGene *> connections;

            /* fast access to the connections of this */
            ConnectionTree connectionsTree;

        public:

            /* return the neurons of this */
            vector<NeuronGene *> getNeurons() const { return neurons; }

            /* returns the connections of this */
            vector<ConnectionGene *> getConnections() const { return connections; }
            
            /* mutate this by inserting a new hidden neuron */
            bool mutateInsertNeuron();

            /* mutate this by creating a new connection */
            bool mutateInsertConnection();

            /* mutate a given percentage of connections */
            void mutateConnections(double percent);

            /* create a new Genotype for the given number of input and output neurons */
            Genotype(const unsigned inputs, const unsigned outputs);

            /* destructor */
            ~Genotype();

#ifdef __DEBUG__ 
            void addNeuron(NeuronGene *gene);
            void addConnection(ConnectionGene *gene);
            static void clearGlobalConnections();
#endif
    };
}

#endif /* __GENOTYPE_H__ */
