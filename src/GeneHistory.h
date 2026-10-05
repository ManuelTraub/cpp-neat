/**
 * implementation of a global gene history, to track
 * similar mutations among neuronal and connection genes
 */
#ifndef __GENE_HISTORY_H__
#define __GENE_HISTORY_H__

#include "Gene.h"
#include "utils/TOGrid.h"
#include "Opts.h"
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

                /* reference to the options of the whole population */
                Opts &opts;

                /* global random value */
                static rand64_t *rand;

                /* delete copy constructor to disalow copies of this */
                GeneHistory(const GeneHistory &) = delete;

                /* the GlobalNeuronGenes of this */
                unordered_map<uint64_t, NeuronGeneConnections> neuronMap;
                
                /* the Global ConnectionGenes of this */
                unordered_map<uint64_t, ConnectionGene> connectionsMap;

                /* the input neurons of this */
                vector<NeuronGene> inputs;
                unordered_map<TOGridIndex, NeuronGene> inputsGrid;

                /* the bias neuron of this */
                vector<NeuronGene> bias;

                /* the output neurons of this */
                vector<NeuronGene> outputs;
                unordered_map<TOGridIndex, NeuronGene> outputsGrid;

                /* the random bias neurons of this */
                vector<NeuronGene> randbias;

                /* the structure genes of this */
                unordered_map<TOGridIndex, StructureGene> structureMap;

                /* creat a new Neuron gene */
                NeuronGene createNeuron(NeuronType type, uint64_t layerID);

            public:

                /* constructor */
                GeneHistory(Opts &opts) : opts(opts.freeze()) { }

                /* insert a new input, bias or output Neuron */
                NeuronGene insertNeuron(NeuronType const type, uint64_t number);

                /* insert a new input or output neuron associated with a 3D position */
                NeuronGene insertNeuron(NeuronType const type, const TOGridIndex index);

                /* insert a new Neuron between the given connection 
                 * Note: the gene must be splitable 
                 */
                NeuronGene insertNeuron(const ConnectionGene &gene);

                /* insert a new input, bias or output Neuron */
                ConnectionGene insertConnection(NeuronGene const &input, NeuronGene const &output);

                /* insert a new StructureGene appending the given Gene */
                StructureGene insertStructure(StructureGene const gene);

                /* insert a initial neuron or muscle tissue */
                StructureGene insertStructure(TissueType const type);

                /* clears all stored genes */
                void clear();

        };
}

#endif /* __GENE_HISTORY_H__ */
