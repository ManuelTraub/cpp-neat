#include "Phenotype.h"
#include <assert.h>
#include <set>
#include <unordered_map>

using namespace NEAT;
using std::set;
using std::unordered_map;
using std::make_pair;

/* create a new Phenotype out of the given Genotype */
Phenotype::Phenotype(Genotype *genome) {

    vector<NeuronGene *> neuronGenes         = genome->getNeurons();
    vector<ConnectionGene *> connectionGenes = genome->getConnections();

    /* create a sorted list of the x values from all 
     * neurons in order to calculate the number of layers 
     */
    set<uint64_t> xValues;

    for (auto &neuron : neuronGenes)
        xValues.insert(neuron->getX());

    /* map a layer index to each different xValue */
    unordered_map<uint64_t, unsigned> layerIndices;
    
    unsigned layerIndex = 0;
    for (auto &value : xValues) {
        layerIndices.insert(make_pair(value, layerIndex));
        layerIndex++;

        /* create the desired number of layers */
        this->neuronLayers.push_back(vector<Neuron *>());
    }

    /* create all neurons and store them efficiently in a QuadTree */
    QuadTree<Neuron> neuronTree;

    for (auto &neuron : neuronGenes) {

        /* store neuron for efficient access */
        Neuron *const newNeuron = new Neuron();
        neuronTree.insert(newNeuron, neuron->getX(), neuron->getY());

        /* store the neuron in its desired layer */
        const unsigned layer = layerIndices[neuron->getX()];
        this->neuronLayers[layer].push_back(newNeuron);
    }

    /* connect the neurons */
    for (auto &connection : connectionGenes) {
        
        Neuron *const input  = neuronTree.get(connection->getInput().getX(), 
                                              connection->getInput().getY());
        Neuron *const output = neuronTree.get(connection->getOutput().getX(), 
                                              connection->getOutput().getY());

        assert(input != NULL && output != NULL);
        output->addInput(input, connection->getWeight());
    }
}

/* calculate the output for a given input */
vector<double> Phenotype::run(vector<double> inputs) {

    vector<double> outputs(this->neuronLayers.back().size());
    
    /* set the input neurons */
    for (unsigned i = 0; i < inputs.size(); i++)
        this->neuronLayers[0][i]->setValue(inputs[i]);
    
    /* calculate the output for each layer */
    for (unsigned l = 0; l < neuronLayers.size(); l++)
        for (unsigned n = 0; n < neuronLayers[l].size(); n++)
            neuronLayers[l][n]->update();

    /* collect the output values */
    for (unsigned i = 0; i < neuronLayers.back().size(); i++)
        outputs[i] = neuronLayers.back()[i]->getValue();

    return outputs;
}
