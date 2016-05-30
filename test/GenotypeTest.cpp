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

TEST_N(basicMutationTest, 1000) {
    
    Genotype genotype(2, 1);

    /* create all posible connections */
    for (unsigned i = 0; i < 1000; i++)
        genotype.mutateInsertConnection();

    /* 2 connections from input to output,
     * 1 connection from bias to output,
     * 1 recurent connetion from output to output
     */
    ASSERT_EQUAL(genotype.getConnections().size(), 4,
                 "more than 4 connections: %lu", 
                 genotype.getConnections().size());
    
    /* insert a new neuron */
    while (!genotype.mutateInsertNeuron());

    ASSERT_EQUAL(genotype.getNeurons().size(), 5,
                 "genotype should have 4 neurons: %lu",
                 genotype.getNeurons().size());

    /* create all posible connections */
    for (unsigned i = 0; i < 1000; i++)
        genotype.mutateInsertConnection();

    /* 3 connections from input and bias to output,
     * 1 recurent connetion from output to output
     * 3 connections from input and bias to the new node
     * 1 recurent connection from output to new node
     * 1 recurent connection from new node to itself
     * 1 connection from the new node to the output
     */
    ASSERT_EQUAL(genotype.getConnections().size(), 10,
                 "more than 10 connections: %lu", 
                 genotype.getConnections().size());

    double weight = genotype.getConnections()[0]->getWeight();

    /* mutate all connections */
    for (unsigned i = 0; i < 1000; i++)
        genotype.mutateConnections(0.5);

    ASSERT_TRUE(weight != genotype.getConnections()[0]->getWeight(),
                "connection should be mutated");

    return args;
}
