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
NeuronGene GeneHistory::insertNeuron(NeuronType const type, uint64_t number) {
    assert(type == NeuronType::INPUT ||
           type == NeuronType::BIAS  ||
           type == NeuronType::OUTPUT);

    if (type == NeuronType::INPUT) {
        while (inputs.size() <= number) {
            inputs.push_back(NeuronGene(NeuronType::INPUT, 0));
            neuronMap.insert({inputs.back().getID(), inputs.back()});
        }
        return inputs[number];

    } else if (type == NeuronType::BIAS) {
        while (bias.size() <= number) {
            bias.push_back(NeuronGene(NeuronType::BIAS, 0));
            neuronMap.insert({bias.back().getID(), bias.back()});
        }
        return bias[number];

    } else if (type == NeuronType::OUTPUT) {
        while (outputs.size() <= number) {
            outputs.push_back(NeuronGene(NeuronType::OUTPUT, UINT64_MAX));
            neuronMap.insert({outputs.back().getID(), outputs.back()});
        }
        return outputs[number];
    }
    
    assert(false);
    return NeuronGene();
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

/* insert a new input, bias or output Neuron */
ConnectionGene GeneHistory::insertConnection(NeuronGene const &input, NeuronGene const &output) {
    
    /* all NeuronGenes have to be in the gene History */
    assert(neuronMap.count(input.getID())  == 1);
    assert(neuronMap.count(output.getID()) == 1);

    if (connectionsTree.contains(input.getID(), output.getID()))
        return connectionsTree(input.getID(), output.getID());

    connectionsTree(input.getID(), output.getID()) = ConnectionGene(input, output);
    return connectionsTree(input.getID(), output.getID());
}

/* clears all stored genes */
void GeneHistory::clear() {
    neuronMap.clear();
    connectionsTree.clear();
    inputs.clear();
    outputs.clear();
    bias.clear();
}
