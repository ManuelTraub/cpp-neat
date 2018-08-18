#include "GeneHistory.h"
#include "Opts.h"

using namespace NEAVC;
using namespace AI;

/* global random value */
rand64_t *GeneHistory::rand = new_rand64_t();

/* creat a new Neuron gene */
NeuronGene GeneHistory::createNeuron(NeuronType type, uint64_t layerID) {

    /* determin the neuron activation function */
    ActivationFunction function = opts.netActivationFunction();
    if (type == NeuronType::OUTPUT && 
        opts.outputActivationFunction() != ActivationFunction::LENGTH) {

        function = opts.outputActivationFunction();
    }

    NeuronGene gene(type, function, layerID);

    /* mutate the neurons activation function if not explecit setted */
    if (opts.randomActivationFunction() && 
        (opts.outputActivationFunction() == ActivationFunction::LENGTH || 
         type != NeuronType::OUTPUT)) {

        gene.mutateFunction();
    }

    return gene;
}

/* insert a new input, bias or output Neuron
 */
NeuronGene GeneHistory::insertNeuron(NeuronType const type, uint64_t number) {

    assert(type == NeuronType::INPUT  ||
           type == NeuronType::BIAS   ||
           type == NeuronType::OUTPUT ||
           type == NeuronType::RAND_BIAS);

    if (type == NeuronType::INPUT) {
        while (inputs.size() <= number) {
            inputs.push_back(createNeuron(NeuronType::INPUT, 0));
            neuronMap.insert({inputs.back().getID(), inputs.back()});
        }
        return inputs[number];

    } else if (type == NeuronType::BIAS) {
        while (bias.size() <= number) {
            bias.push_back(createNeuron(NeuronType::BIAS, 0));
            neuronMap.insert({bias.back().getID(), bias.back()});
        }
        return bias[number];

    } else if (type == NeuronType::OUTPUT) {
        while (outputs.size() <= number) {
            outputs.push_back(createNeuron(NeuronType::OUTPUT, UINT64_MAX));
            neuronMap.insert({outputs.back().getID(), outputs.back()});
        }
        return outputs[number];

    } else if (type == NeuronType::RAND_BIAS) {
        while (randbias.size() <= number) {
            randbias.push_back(createNeuron(NeuronType::RAND_BIAS, 0));
            neuronMap.insert({randbias.back().getID(), randbias.back()});
        }
        return randbias[number];
    }
    
    assert(false);
    return createNeuron(NeuronType::BIAS, 0);
}

/* insert a new input or output neuron associated with a 3D position */
NeuronGene GeneHistory::insertNeuron(NeuronType const type, const TOGridIndex index) {

    assert(type == NeuronType::INPUT ||
           type == NeuronType::OUTPUT);

    if (type == NeuronType::INPUT) {
        if (!inputsGrid.count(index)) {
            inputsGrid.insert({index, createNeuron(NeuronType::INPUT, 0)});
            neuronMap.insert({inputsGrid.at(index).getID(), inputsGrid.at(index)});
        }
        return inputsGrid.at(index);

    } else if (type == NeuronType::OUTPUT) {
        if (!outputsGrid.count(index)) {
            outputsGrid.insert({index, createNeuron(NeuronType::OUTPUT, UINT64_MAX)});
            neuronMap.insert({outputsGrid.at(index).getID(), outputsGrid.at(index)});
        }
        return outputsGrid.at(index);
    }
    
    assert(false);
    return createNeuron(NeuronType::BIAS, 0);

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
    NeuronGene newNeuron = gene.split(opts.netActivationFunction());

    /* mutate the neurons activation function if not explecit setted */
    if (opts.randomActivationFunction())
        newNeuron.mutateFunction();

    inputConnections.addOutput(newNeuron.getID());
    outputConnections.addInput(newNeuron.getID());

    neuronMap.insert({newNeuron.getID(), NeuronGeneConnections(newNeuron)});

    neuronMap.at(newNeuron.getID()).addInput(inpuID);
    neuronMap.at(newNeuron.getID()).addOutput(outputID);

    return newNeuron;
}

/* insert a new input, bias or output Neuron */
ConnectionGene GeneHistory::insertConnection(NeuronGene const &input, NeuronGene const &output) {
    
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

/* insert a new StructureGene appending the given Gene */
StructureGene GeneHistory::insertStructure(StructureGene const gene) {

    /* create an adjacent structure index */
    const TOGridIndex index = gene.getIndex();
    TOGridIndex newIndex = index.getAjacent(rand64(rand) % 14);
    
    /* only insert a new StructureGene if there isn't allready one */
    if (!structureMap.count(newIndex)) {

        StructureGene gene(
            TissueType::TISSUE,
            newIndex.getX(),
            newIndex.getY(),
            newIndex.getZ(),
            rand_range_d64(rand, opts.creationSizeRangeStart(), opts.creationSizeRangeEnd())
        );
        structureMap.insert({newIndex, gene});
    }

    return structureMap.at(newIndex);
}

/* insert a initial neuron or muscle tissue */
StructureGene GeneHistory::insertStructure(TissueType const type) {
    
    if (type == TissueType::NEURON) {
        TOGridIndex index;

        /* only insert a new StructureGene if there isn't allready one */
        if (!structureMap.count(index)) {

            StructureGene gene(
                TissueType::NEURON,
                index.getX(),
                index.getY(),
                index.getZ(),
                rand_range_d64(rand, opts.creationSizeRangeStart(), opts.creationSizeRangeEnd())
            );
            structureMap.insert({index, gene});
        }

        return structureMap.at(index);
    }

    assert(type == TissueType::MUSCLE);
    assert(structureMap.count(TOGridIndex()));

    StructureGene gene = insertStructure(structureMap.at(TOGridIndex()));
    gene.setType(TissueType::MUSCLE);
    return gene;
}

/* clears all stored genes */
void GeneHistory::clear() {
    neuronMap.clear();
    connectionsMap.clear();
    inputs.clear();
    outputs.clear();
    bias.clear();
}
