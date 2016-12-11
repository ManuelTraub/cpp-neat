/**
 * Abstract Genotype encoding a Individual within NEAVC
 */
#ifndef __GENOTYPE_H__
#define __GENOTYPE_H__

#include <vector>
#include <unordered_map>
#include "../utils/utils.h"
#include "Gene.h"
#include "GeneHistory.h"
#include "Opts.h"

using std::unordered_map;

namespace NEAVC {

    namespace AI {
        
        class Genotype {
            
            private:

                /* reference to the options of the whole population */
                Opts &opts;

                /* global random value to mutate this */
                static rand64_t *rand;

                /* the neurons of this */
                unordered_map<uint64_t, NeuronGene> neurons;

                /* the ConnectionGenes of this */
                unordered_map<uint64_t, ConnectionGene> connections;

                /* the GeneHistory reference of this */
                static GeneHistory &history;

                /* the latest gene generation number */
                uint64_t generation;

                /* recombinate using the weight of similar genes from a random parrent */
                void recombinateRandomChoose(const Genotype &fitter, const Genotype &other);

                /* recombinate averaging the weights of similar genes */
                void recombinateAverage(const Genotype &fitter, const Genotype &other);

            public:

                /* return the neurons of this */
                unordered_map<uint64_t, NeuronGene> getNeurons() const { return neurons; }

                /* returns the connections of this */
                unordered_map<uint64_t, ConnectionGene> getConnections() const { return connections; }
                
                /* mutate this by inserting a new hidden neuron */
                bool mutateInsertNeuron();

                /* mutate this by creating a new connection */
                bool mutateInsertConnection();

                /* mutate a given percentage of connections */
                void mutateConnections();

                /* mutate a given percentage of the neurons of this */
                void mutateNeurons();

                /* create a new Genotype for the given number of input and output neurons */
                Genotype(const unsigned inputs, const unsigned outputs, Opts &opts);

                /* creat a new Genotype mixing the genes of better with other */
                Genotype(const Genotype &fitter, const Genotype &other, Opts &opts);

                /* calculates the similarity with this and other */
                double similarity(const Genotype &other) const;

                /* returns the genome age of this (GenerationCounter - jungest gene) */
                uint64_t genomeAge() const { return GenerationCounter::get() - generation; }

                /* create the initial connections which 
                 * fully connects the input and output Neurons of this 
                 */
                void createInitialConnections(bool connectBias = true);

#ifdef __DEBUG__ 
                NeuronGene &addNeuron(const ConnectionGene &gene);
                ConnectionGene &addConnection(const NeuronGene &gene1, const NeuronGene &gene2);
#endif
        };
    }
}

#endif /* __GENOTYPE_H__ */
