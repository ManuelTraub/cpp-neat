#include "Phenotype.h"
#include <assert.h>

using namespace NEAT;

/* create a new Phenotype out of the given Genotype */
Phenotype::Phenotype(Genotype *genome) {

    vector<NeuronGene *> neuronGenes         = genome->getNeurons();
    vector<ConnectionGene *> connectionGenes = genome->getConnections();
    vector<Neuron *> neuronsById;
    
    /* creat the layers */
    for (unsigned i = 0; i < genome->getNumberLayers(); i++)
        this->neuronLayers.push_back(vector<Neuron *>());

    /* create the neurons */
    for (unsigned i = 0; i < neuronGenes.size(); i++) {
        
        const NeuronGene *gene = neuronGenes[i];
        neuronsById.push_back(new Neuron());

        if (gene->type == NeuronType::INPUT)
            this->neuronLayers[0].push_back(neuronsById.back());
        else if (gene->type == NeuronType::OUTPUT)
            this->neuronLayers.back().push_back(neuronsById.back());
        else
            this->neuronLayers[gene->layer].push_back(neuronsById.back());
    }

    /* connect the neurons */
    for (unsigned i = 0; i < connectionGenes.size(); i++) {
        
        const ConnectionGene *gene = connectionGenes[i];
        const unsigned output = gene->outputNeuron;
        const unsigned input  = gene->inputNeuron;

        neuronsById[output]->addInput(neuronsById[input], gene->getWeight());
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
