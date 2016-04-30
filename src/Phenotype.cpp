#include "Phenotype.h"
#include <assert.h>

using namespace NEAT;

/* calculate the output for a given input */
vector<double> Phenotype::run(vector<double> inputs) {
    
    /* set the input neurons to easily calulate the 
     * other neuron outputs through the connections */
    for (unsigned i = 0; i < inputs.size(); i++)
        genome.inputNeurons[i].setOutput(inputs[i]);
    
    for (unsigned l = 0; l < genome.connections.size(); l++) {

        /* calculate the inputs for this layer */
        for (unsigned c = 0; c < genome.connections[l].size(); c++) {

            const ConnectionGenes &currentGene = genome.connections[l][c];
            const unsigned inputNeuron  = currentGene.getInputNeuron();
            const unsigned inputLayer   = currentGene.getInputLayer();
            const unsigned outputNeuron = currentGene.getOutputNeuron();
            const double   weight       = currentGene.getWeight();
            const double   inputValue   = genome.neurons[inputLayer][inputNeuron].getOutput();

            /* add an input for the output neuron */
            genome.neuron[l + 1][outputNeuron].addInput(weight * inputValue);

#ifdef DEBUG
            /* check that the output layer maches */
            if (currentGene.getOutputLayer() != l + 1)
                assert(false);
#endif
        }


}
