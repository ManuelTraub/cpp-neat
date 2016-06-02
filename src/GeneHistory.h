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

            /* generate a dummy placeholder gene */
            NeuronGeneConnections() { }

    };

    class GeneHistory {

        private:

            /* the GlobalNeuronGenes of this */
            unordered_map<uint64_t, NeuronGeneConnections> neuronMap;

        public:

            /* insert a new input, bias or output Neuron
             */
            void insertNeuron(NeuronGene const &gene) {
                assert(gene.getType() == NeuronType::INPUT ||
                       gene.getType() == NeuronType::BIAS  ||
                       gene.getType() == NeuronType::OUTPUT);

                neuronMap.insert(make_pair(gene.getID(), NeuronGeneConnections(gene)));
            }

            /* insert a new Neuron between the given connection 
             * Note: the gene must be splitable 
             */
            NeuronGene insertNeuron(const ConnectionGene &gene) {
                assert(gene.canBeSplitted());
                
                /* check if an Neuron already exists */
                const uint64_t inpuID   = gene.getInput().getID();
                const uint64_t outputID = gene.getOutput().getID();

                NeuronGeneConnections &inputConnections  = neuronMap[inpuID];
                NeuronGeneConnections &outputConnections = neuronMap[outputID];

                /* checks if the connection was allready splitted by checkign for and
                 * Neuron with conected to input and output of the connection
                 */
                for (uint64_t &inID : inputConnections.getOutputs()) {
                    for (uint64_t &outID : outputConnections.getInputs()) {
                        if (inID == outID)
                            return neuronMap[inID].getGene();
                    }
                }

                /* if no connection was found insert a new one */
                NeuronGene newNeuron = gene.split();

                inputConnections.addOutput(newNeuron.getID());
                outputConnections.addInput(newNeuron.getID());

                neuronMap.insert(make_pair(newNeuron.getID(), NeuronGeneConnections(newNeuron)));

                neuronMap[newNeuron.getID()].addInput(inpuID);
                neuronMap[newNeuron.getID()].addOutput(outputID);

                return newNeuron;
            }

    };

}

#endif /* __GENE_HISTORY_H__ */
