/**
 * implementation of a global gene history, to track
 * similar mutations among neuronal and connection genes
 */
#ifndef __GENE_HISTORY_H__
#define __GENE_HISTORY_H__

#include "Gene.h"
#include <vector>
#include <unordered_map>

using std::vector;
using std::unordered_map;
using std::make_pair;

namespace NEAT {
    
    /* Connections of a NeuronGene */
    class NeuronGeneConnections {

        private:

            /* the NeuronGene of this */
            NeuronGene gene;

            /* the input connections of this NeuronGene */
            vector<uint64_t> inputs;

            /* the output connections of this NeuronGene */
            vector<uint64_t> outputs;

        public:

            /* returns the inputs of this */
            vector<uint64_t> getInputs() const { return inputs; }

            /* returns the outputs of this */
            vector<uint64_t> getOutputs() const { return outputs; }

            /* add an input id */
            void addInput(const uint64_t id) { inputs.push_back(id); }

            /* add an output id */
            void addOutput(const uint64_t id) { outputs.push_back(id); }

            /* returns the NeuronGene of this */
            NeuronGene getGene() const { return gene; }

            /* constructor */
            NeuronGeneConnections(const NeuronGene &gene) : gene(gene) { }

    };

    class GeneHistory {

        private:

            /* the only instance of the GeneHistory */
            static GeneHistory onlyInstance;

            /* private constructor and destructor to make this a singleton */
            GeneHistory() { }

            /* private destructor to avoid deletion of the singleton */
            ~GeneHistory() { }

            /* delete copy constructor to disalow copies of this */
            GeneHistory(const GeneHistory &) = delete;

            /* the GlobalNeuronGenes of this */
            unordered_map<uint64_t, NeuronGeneConnections> neuronMap;
            
            /* the Global ConnectionGenes of this */
            unordered_map<uint64_t, ConnectionGene> connectionsMap;

            /* the input neurons of this */
            vector<NeuronGene> inputs;

            /* the bias Neuron of this */
            vector<NeuronGene> bias;

            /* the output neurons of this */
            vector<NeuronGene> outputs;

        public:

            /* access the only instance of this */
            static GeneHistory &getInstance();

            /* insert a new input, bias or output Neuron */
            NeuronGene insertNeuron(NeuronType const type, uint64_t number);

            /* insert a new Neuron between the given connection 
             * Note: the gene must be splitable 
             */
            NeuronGene insertNeuron(const ConnectionGene &gene);

            /* insert a new input, bias or output Neuron */
            ConnectionGene insertConnection(NeuronGene const &input, NeuronGene const &output);

            /* clears all stored genes */
            void clear();

    };

}

#endif /* __GENE_HISTORY_H__ */
