#include "Phenotype.h"
#include <assert.h>

using namespace NEAT;

/* create a new Phenotype out of the given Genotype */
Phenotype::Phenotype(Genotype *genome) {

    vector<NeuronGene *> neuronGenes         = genome->getNeurons();
    vector<ConnectionGene *> connectionGenes = genome->getConnections();
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
