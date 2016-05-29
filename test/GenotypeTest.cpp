#include "../src/utils.h"
#include "../src/Genotype.h"

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
    Genotype::clearGlobalConnections();
    return args;
}

AFTER() {
    (void) args;
}

TEST(basicTest) {
    
    Genotype genotype1(2, 2);
    Genotype genotype2(2, 2);

    vector<NeuronGene *> neurons = genotype1.getNeurons();

    genotype1.addConnection(new ConnectionGene(*neurons[0], *neurons[3]));
    genotype2.addConnection(new ConnectionGene(*neurons[0], *neurons[3]));

    ASSERT_EQUAL(genotype1.getConnections()[0]->getID(),
                 genotype2.getConnections()[0]->getID(),
                 "Historical ids unequal: %lu != %lu",
                 genotype1.getConnections()[0]->getID(),
                 genotype2.getConnections()[0]->getID());

    genotype1.addConnection(new ConnectionGene(*neurons[1], *neurons[3]));
    genotype2.addConnection(new ConnectionGene(*neurons[1], *neurons[2]));

    ASSERT_TRUE(genotype1.getConnections()[1]->getID() !=
                genotype2.getConnections()[1]->getID(),
                 "Historical ids shouldn't be equal: %lu == %lu",
                 genotype1.getConnections()[1]->getID(),
                 genotype2.getConnections()[1]->getID());

    return args;
}

TEST(basicMutationTest) {
    
    Genotype genotype(2, 1);

    for (unsigned i = 0; i < 100; i++)
        genotype.mutateInsertConnection();

    /* 2 connections from input to output,
     * 1 connection from bias to output,
     * 1 recurent connetion from output to output
     */
    ASSERT_EQUAL(genotype.getConnections().size(), 4,
                 "more than two connections: %lu", 
                 genotype.getConnections().size());
    
    /* insert a new neuron */
    while (!genotype.mutateInsertNeuron());

    ASSERT_EQUAL(genotype.getNeurons().size(), 5,
                 "genotype should have 4 neurons: %lu",
                 genotype.getNeurons().size());

    return args;
}
