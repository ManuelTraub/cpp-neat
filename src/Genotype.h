/**
 * Abstract Genotype encoding a Individual within NEAT
 */
#ifndef __GENOTYPE_H__
#define __GENOTYPE_H__

#include <vector>
#include <unordered_map>
#include "utils.h"
#include "Gene.h"
#include "GeneHistory.h"

using std::unordered_map;

namespace NEAT {
    
    class Genotype {
        
        private:

            /* global random value to mutate this */
            static rand64_t *rand;

            /* the neurons of this */
            unordered_map<uint64_t, NeuronGene> neurons;

            /* the ConnectionGenes of this */
            unordered_map<uint64_t, ConnectionGene> connections;

            /* the GeneHistory reference of this */
            GeneHistory &history;

            /* the latest gene generation number */
            uint64_t generation;

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
            void mutateConnections(double percent);

            /* mutate this by disabeling a connection */
            bool mutateDisableConnection();

            /* create a new Genotype for the given number of input and output neurons */
            Genotype(const unsigned inputs, const unsigned outputs);

            /* creat a new Genotype mixing the genes of better with other */
            Genotype(const Genotype &fitter, const Genotype &other);

            /* calculates the similarity with this and other */
            double similarity(const Genotype &other);

            /* the gene multiplayer for calculating similarity */
            static double geneMultiplier;

            /* avg weight diference multiplier when calculating the similarity */
            static double weightMultiplier;

            /* returns the genome age of this (GenerationCounter - jungest gene) */
            uint64_t genomeAge() const { return GenerationCounter::get() - generation; }

#ifdef __DEBUG__ 
            NeuronGene &addNeuron(const ConnectionGene &gene);
            ConnectionGene &addConnection(const NeuronGene &gene1, const NeuronGene &gene2);
#endif
    };
}

#endif /* __GENOTYPE_H__ */
