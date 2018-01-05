#include "../../src/utils/utils.h"
#include "../../src/AI/GeneHistory.h"

#include <vector>

using std::vector;
using namespace NEAVC;
using namespace AI;

#define static_t rand64_t
#define test_t rand64_t
#include "../TestSuite.h"


BEVOR_ALL() {
    return new_rand64_t();
}

AFTER_ALL() {
    free(args);
}

BEVOR() {
    return args;
}

AFTER() {
    (void) args;
}

TEST(basicTest) {
    
    Opts opts;
    GeneHistory geneHistory(opts);

    NeuronGene input  = geneHistory.insertNeuron(NeuronType::INPUT, 0);
    NeuronGene output = geneHistory.insertNeuron(NeuronType::OUTPUT, 0);

    ConnectionGene connection(input, output);

    NeuronGene neuron1 = geneHistory.insertNeuron(connection);
    NeuronGene neuron2 = geneHistory.insertNeuron(
        ConnectionGene(connection.getInput(), connection.getOutput())
    );
    ASSERT_EQUAL(neuron1.getID(), neuron2.getID(), "neuron genes not equal");

    ConnectionGene connection1 = geneHistory.insertConnection(input, output);
    ConnectionGene connection2 = geneHistory.insertConnection(input, output);

    ASSERT_EQUAL(connection1.getID(), connection2.getID(), "ids should be equal");
    
    StructureGene structure = geneHistory.insertStructure(TissueType::NEURON);
    unordered_map<TOGridIndex, StructureGene> structureMap;
    for (unsigned i = 0; i < 1000; i++) {
        StructureGene s = geneHistory.insertStructure(structure);

        TOGridIndex index = s.getIndex();
        if (structureMap.count(index)) { 
            ASSERT_EQUAL(structureMap.at(index).getID(), s.getID(), "ids not equal");
        } else
            structureMap.insert({index, s});
    }

    ASSERT_EQUAL(structureMap.size(), 14, "there should be 14 adjacent structures");

    return args;
}
