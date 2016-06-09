#include "../src/utils.h"
#include "../src/GeneHistory.h"

#include <vector>

using std::vector;
using namespace NEAT;

#define static_t rand64_t
#define test_t rand64_t
#include "TestSuite.h"


BEVOR_ALL() {
    return new_rand64_t(time(NULL));
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
    
    GeneHistory &history = GeneHistory::getInstance();

    NeuronGene input  = history.insertNeuron(NeuronType::INPUT, 0);
    NeuronGene output = history.insertNeuron(NeuronType::OUTPUT, 0);

    ConnectionGene connection(input, output);

    NeuronGene neuron1 = history.insertNeuron(connection);
    NeuronGene neuron2 = history.insertNeuron(ConnectionGene(connection.getInput(), 
                                                             connection.getOutput()));
    ASSERT_EQUAL(neuron1.getID(), neuron2.getID(), "neuron genes not equal");

    ConnectionGene connection1 = history.insertConnection(input, output);
    ConnectionGene connection2 = history.insertConnection(input, output);

    ASSERT_EQUAL(connection1.getID(), connection2.getID(), "ids should be equal");
    return args;
}
