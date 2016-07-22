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
    return NeuronGene(NeuronType::BIAS, 0);
}

/* insert a new Neuron between the given connection 
 * Note: the gene must be splitable 
 */
NeuronGene GeneHistory::insertNeuron(const ConnectionGene &gene) {

    for (const auto &in : curConnections) {
        if (in.getInput() == gene.getInput()) {
            for (const auto &out : curConnections) {
                if (out.getOutput() == gene.getOutput()) {
                    if (in.getOutput() == out.getInput()) {
                        return in.getOutput();             
                    }
                }
            }
        }
    }
    NeuronGene newNeuron = gene.split();
    curNeurons.push_back(newNeuron);
    return newNeuron;


    assert(gene.canBeSplitted());
    
    /* check if an Neuron already exists */
    const uint64_t inpuID   = gene.getInput().getID();
    const uint64_t outputID = gene.getOutput().getID();

    /* all NeuronGenes have to be in the gene History */
    assert(neuronMap.count(inpuID)   == 1);
    assert(neuronMap.count(outputID) == 1);

    NeuronGeneConnections &inputConnections  = neuronMap.at(inpuID);
    NeuronGeneConnections &outputConnections = neuronMap.at(outputID);

    /* checks if the connection was allready splitted by checkign for and
     * Neuron with conected to input and output of the connection
     */
    for (uint64_t &inID : inputConnections.getOutputs()) {
        for (uint64_t &outID : outputConnections.getInputs()) {
            if (inID == outID)
                return neuronMap.at(inID).getGene();
        }
    }

    /* if no connection was found insert a new one */
//    NeuronGene newNeuron = gene.split();

    inputConnections.addOutput(newNeuron.getID());
    outputConnections.addInput(newNeuron.getID());

    neuronMap.insert({newNeuron.getID(), NeuronGeneConnections(newNeuron)});

    neuronMap.at(newNeuron.getID()).addInput(inpuID);
    neuronMap.at(newNeuron.getID()).addOutput(outputID);

    return newNeuron;
}

/* insert a new input, bias or output Neuron */
ConnectionGene GeneHistory::insertConnection(NeuronGene const &input, NeuronGene const &output) {

    for (const auto &c : curConnections)
        if (c.getInput() == input && c.getOutput() == output)
            return c;

    ConnectionGene newGene(input, output);
    curConnections.push_back(newGene);
    return newGene;

    
    /* all NeuronGenes have to be in the gene History */
    assert(neuronMap.count(input.getID())  == 1);
    assert(neuronMap.count(output.getID()) == 1);

    /* to map the input and output id to an single number 
     * they both need to be smaler than 2^32 */
    assert(input.getID() < UINT32_MAX && output.getID() < UINT32_MAX);

    /* convert the input and output id to a unique uint64_t value */
    uint64_t geneID = input.getID() | (output.getID() << 32);

    /* only insert a new ConnectionGene if there isn't allready one 
     * with the same input and output id
     */
    if (!connectionsMap.count(geneID))
        connectionsMap.insert({geneID, ConnectionGene(input, output)});

    return connectionsMap.at(geneID);
}

/* clears all stored genes */
void GeneHistory::clear() {
    neuronMap.clear();
    connectionsMap.clear();
    inputs.clear();
    outputs.clear();
    bias.clear();
}
