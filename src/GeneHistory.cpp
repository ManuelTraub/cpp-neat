#include "GeneHistory.h"
using namespace NEAT;

/* the only instance of the GeneHistory */
GeneHistory GeneHistory::onlyInstance;

/* access the only instance of this */
GeneHistory &GeneHistory::getInstance() {
    return onlyInstance;
}

/* insert a new input, bias or output Neuron
 */
void GeneHistory::insertNeuron(NeuronGene const &gene) {
    assert(gene.getType() == NeuronType::INPUT ||
           gene.getType() == NeuronType::BIAS  ||
           gene.getType() == NeuronType::OUTPUT);

    neuronMap.insert(make_pair(gene.getID(), NeuronGeneConnections(gene)));
}

/* insert a new Neuron between the given connection 
 * Note: the gene must be splitable 
 */
NeuronGene GeneHistory::insertNeuron(const ConnectionGene &gene) {
    assert(gene.canBeSplitted());
    
    /* check if an Neuron already exists */
    const uint64_t inpuID   = gene.getInput().getID();
    const uint64_t outputID = gene.getOutput().getID();

    /* all NeuronGenes have to be in the gene History */
    assert(neuronMap.count(inpuID)   == 1);
    assert(neuronMap.count(outputID) == 1);

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
