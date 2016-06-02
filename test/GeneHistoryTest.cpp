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
    
    GeneHistory history;

    ConnectionGene connection(NeuronGene(NeuronType::INPUT, 0), 
                              NeuronGene(NeuronType::OUTPUT, UINT64_MAX));

    history.insertNeuron(connection.getInput());
    history.insertNeuron(connection.getOutput());
    NeuronGene neuron1 = history.insertNeuron(connection);
    NeuronGene neuron2 = history.insertNeuron(ConnectionGene(connection.getInput(), 
                                                             connection.getOutput()));

    ASSERT_EQUAL(neuron1.getID(), neuron2.getID(), "neuron genes not equal");
    return args;
}
