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
    GeneHistory::getInstance().clear();
    return args;
}

AFTER() {
    (void) args;
}

TEST(basicTest) {
    
    Genotype genotype1(2, 2);
    Genotype genotype2(2, 2);
    Genotype genotype3(2, 2);
    Genotype genotype4(2, 2);

    /* sort the neurons by input and output */
    unordered_map<uint64_t, NeuronGene> neuronsMap1 = genotype1.getNeurons();
    unordered_map<uint64_t, NeuronGene> neuronsMap2 = genotype2.getNeurons();
    vector<NeuronGene> inputs1, inputs2;
    vector<NeuronGene> outputs1, outputs2;
    
    for (const auto &e : neuronsMap1) {
        if (e.second.getType() == NeuronType::INPUT || 
            e.second.getType() == NeuronType::BIAS)
            inputs1.push_back(e.second);
        else if (e.second.getType() == NeuronType::OUTPUT)
            outputs1.push_back(e.second);
        else
            ASSERT_FAIL("network should not have hidden neurons");
    }

    for (const auto &e : neuronsMap2) {
        if (e.second.getType() == NeuronType::INPUT || 
            e.second.getType() == NeuronType::BIAS)
            inputs2.push_back(e.second);
        else if (e.second.getType() == NeuronType::OUTPUT)
            outputs2.push_back(e.second);
        else
            ASSERT_FAIL("network should not have hidden neurons");
    }

    ASSERT_EQUAL(inputs1.size(), 3, "more than 3 inputs");
    ASSERT_EQUAL(outputs1.size(), 2, "more than 2 outputs");
    ASSERT_EQUAL(inputs2.size(), 3, "more than 3 inputs");
    ASSERT_EQUAL(outputs2.size(), 2, "more than 2 outputs");

    genotype1.addConnection(inputs1[0], outputs1[0]);
    genotype2.addConnection(inputs2[0], outputs2[0]);

    unordered_map<uint64_t, ConnectionGene> connections1 = genotype1.getConnections();
    unordered_map<uint64_t, ConnectionGene> connections2 = genotype2.getConnections();

    ASSERT_EQUAL(connections1.size(), 1, "more than one connection");
    ASSERT_EQUAL(connections2.size(), 1, "more than one connection");

    ASSERT_EQUAL(connections1.begin()->second.getID(),
                 connections2.begin()->second.getID(),
                 "Historical ids unequal: %lu != %lu",
                 connections1.begin()->second.getID(),
                 connections2.begin()->second.getID());
    

    genotype3.addConnection(inputs1[0], outputs1[1]);
    genotype4.addConnection(inputs2[1], outputs2[0]);

    connections1 = genotype3.getConnections();
    connections2 = genotype4.getConnections();

    ASSERT_EQUAL(connections1.size(), 1, "more than one connection");
    ASSERT_EQUAL(connections2.size(), 1, "more than one connection");

    ASSERT_TRUE(connections1.begin()->second.getID() !=
                connections2.begin()->second.getID(),
                 "Historical ids shouldn't be equal: %lu == %lu",
                 connections1.begin()->second.getID(),
                 connections2.begin()->second.getID());

    return args;
}

TEST(basic2DTest) {
    
    Genotype genotype1(1, 2);
    Genotype genotype2(1, 2);

    unordered_map<uint64_t, NeuronGene> neuronsMap = genotype1.getNeurons();
    vector<NeuronGene> inputs;
    vector<NeuronGene> outputs;
    
    for (const auto &e : neuronsMap) {
        if (e.second.getType() == NeuronType::INPUT || 
            e.second.getType() == NeuronType::BIAS)
            inputs.push_back(e.second);
        else if (e.second.getType() == NeuronType::OUTPUT)
            outputs.push_back(e.second);
        else
            ASSERT_FAIL("network should not have hidden neurons");
    }

    ASSERT_EQUAL(inputs.size(), 2, "more than 2 inputs");
    ASSERT_EQUAL(outputs.size(), 2, "more than 2 outputs");

    genotype1.addConnection(inputs[0], outputs[1]);
    genotype2.addConnection(inputs[1], outputs[0]);

    unordered_map<uint64_t, ConnectionGene> connections1 = genotype1.getConnections();
    unordered_map<uint64_t, ConnectionGene> connections2 = genotype2.getConnections();

    ASSERT_EQUAL(connections1.size(), 1, "more than one connection");
    ASSERT_EQUAL(connections2.size(), 1, "more than one connection");

    ASSERT_TRUE(connections1.begin()->second.getID() !=
                connections2.begin()->second.getID(),
                "Historical ids equal: %lu == %lu",
                connections1.begin()->second.getID(),
                connections2.begin()->second.getID());
    

    genotype1.addNeuron(connections1.begin()->second);
    genotype2.addNeuron(connections2.begin()->second);

    unordered_map<uint64_t, NeuronGene> neurons1 = genotype1.getNeurons();
    unordered_map<uint64_t, NeuronGene> neurons2 = genotype2.getNeurons();

    ASSERT_EQUAL(neurons1.size(), 5, "more than 5 neurons");
    ASSERT_EQUAL(neurons2.size(), 5, "more than 5 neurons");

    NeuronGene hidden1, hidden2;

    for (const auto &e : neurons1)
        if (e.second.getType() == NeuronType::HIDDEN)
            hidden1 = e.second;

    for (const auto &e : neurons2)
        if (e.second.getType() == NeuronType::HIDDEN)
            hidden2 = e.second;

    ASSERT_TRUE(hidden1.getID() != hidden2.getID(),
                "Historical ids shouldn't be equal: %lu == %lu",
                hidden1.getID(), hidden2.getID());


    return args;
}

TEST(basicInLayerTestTest) {
    
    Genotype genotype1(1, 2);
    Genotype genotype2(1, 2);

    unordered_map<uint64_t, NeuronGene> neuronsMap = genotype1.getNeurons();
    vector<NeuronGene> inputs;
    vector<NeuronGene> outputs;
    
    for (const auto &e : neuronsMap) {
        if (e.second.getType() == NeuronType::INPUT || 
            e.second.getType() == NeuronType::BIAS)
            inputs.push_back(e.second);
        else if (e.second.getType() == NeuronType::OUTPUT)
            outputs.push_back(e.second);
        else
            ASSERT_FAIL("network should not have hidden neurons");
    }

    ASSERT_EQUAL(inputs.size(), 2, "more than 2 inputs");
    ASSERT_EQUAL(outputs.size(), 2, "more than 2 outputs");

    genotype1.addConnection(inputs[0], outputs[1]);
    genotype1.addConnection(inputs[1], outputs[0]);
    genotype2.addConnection(inputs[0], outputs[1]);
    genotype2.addConnection(inputs[1], outputs[0]);

    unordered_map<uint64_t, ConnectionGene> connections1 = genotype1.getConnections();
    unordered_map<uint64_t, ConnectionGene> connections2 = genotype2.getConnections();

    ASSERT_EQUAL(connections1.size(), 2, "more than one connection");
    ASSERT_EQUAL(connections2.size(), 2, "more than one connection");

    genotype1.addNeuron(connections1.begin()->second);
    genotype1.addNeuron((++connections1.begin())->second);
    genotype2.addNeuron(connections2.begin()->second);
    genotype2.addNeuron((++connections2.begin())->second);

    unordered_map<uint64_t, NeuronGene> neurons1 = genotype1.getNeurons();
    unordered_map<uint64_t, NeuronGene> neurons2 = genotype2.getNeurons();

    ASSERT_EQUAL(neurons1.size(), 6, "more than 6 neurons");
    ASSERT_EQUAL(neurons2.size(), 6, "more than 6 neurons");

    vector<NeuronGene> hidden1, hidden2;

    for (const auto &e : neurons1)
        if (e.second.getType() == NeuronType::HIDDEN)
            hidden1.push_back(e.second);

    for (const auto &e : neurons2)
        if (e.second.getType() == NeuronType::HIDDEN)
            hidden2.push_back(e.second);

    ASSERT_EQUAL(hidden1.size(), 2, "more than 2 hidden neurons");
    ASSERT_EQUAL(hidden2.size(), 2, "more than 2 hidden neurons");

    /* switch neurons if they are not in the same order */
    if (hidden1[0].getID() != hidden2[0].getID()) {
        NeuronGene tmp = hidden1[0];
        hidden1[0] = hidden1[1];
        hidden1[1] = tmp;
    }

    ASSERT_EQUAL(hidden1[0].getID(), hidden2[0].getID(), "ids sould be equal");
    ASSERT_EQUAL(hidden1[1].getID(), hidden2[1].getID(), "ids sould be equal");

    ConnectionGene connection1 = genotype1.addConnection(hidden1[0], hidden1[1]);
    ConnectionGene connection2 = genotype2.addConnection(hidden2[0], hidden2[1]);

    ASSERT_EQUAL(connection1.getID(), connection2.getID(), "conections hsould be equal");

    genotype1.addNeuron(connection1);
    genotype2.addNeuron(connection2);

    neurons1 = genotype1.getNeurons();
    neurons2 = genotype2.getNeurons();
    connections1 = genotype1.getConnections();
    connections2 = genotype2.getConnections();
    
    ASSERT_EQUAL(neurons1.size(), 7, "should be 7 neurons");
    ASSERT_EQUAL(neurons2.size(), 7, "should be 7 neurons");
    ASSERT_EQUAL(connections1.size(), 3, "should be 3 connections: %lu", connections1.size());
    ASSERT_EQUAL(connections2.size(), 3, "should be 3 connections: %lu", connections2.size());

    /* neurons of both genotypes should be the same */
    ASSERT_EQUAL(neurons1.size(), neurons2.size(), "not the same number of neurons");
    for (const auto &e : neurons1)
        ASSERT_EQUAL(neurons2.count(e.second.getID()), 1, "neuron not found");

    /* all connections sould be equal */
    ASSERT_EQUAL(connections1.size(), connections2.size(), "not the same number of connections");
    for (const auto &e : connections1)
        ASSERT_EQUAL(connections2.count(e.second.getID()), 1, "connection not found");

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
                 "genotype should have 5 neurons: %lu",
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

    uint64_t key  = genotype.getConnections().begin()->first;
    double weight = genotype.getConnections()[key].getWeight();

    /* mutate all connections */
    for (unsigned i = 0; i < 1000; i++)
        genotype.mutateConnections(0.5);

    ASSERT_TRUE(weight != genotype.getConnections()[key].getWeight(),
                "connection should be mutated");

    return args;
}

TEST_N(advancedMutationTest, 100) {

    uint64_t nInputs  = rand_range64(args, 1, 10);
    uint64_t nOutputs = rand_range64(args, 1, 10);
    
    /* recurent connection on each output
     * from each input + bias a connection to each output
     * a connection from each output to al other outputs
     */
    uint64_t nConnections = nOutputs + (nInputs + 1) * nOutputs + nOutputs * (nOutputs - 1);
    
    Genotype genotype1(nInputs, nOutputs);
    Genotype genotype2(nInputs, nOutputs);

    /* create all posible connections */
    for (unsigned i = 0; i < nConnections * 100; i++) {
        genotype1.mutateInsertConnection();
        genotype2.mutateInsertConnection();
    }

    ASSERT_EQUAL(genotype1.getConnections().size(), 
                 nConnections,
                 "more than %lu connections: %lu",
                 nConnections,
                 genotype1.getConnections().size());
    
    ASSERT_EQUAL(genotype2.getConnections().size(), 
                 nConnections,
                 "more than %lu connections: %lu",
                 nConnections,
                 genotype2.getConnections().size());

    unordered_map<uint64_t, NeuronGene> neurons1 = genotype1.getNeurons();
    unordered_map<uint64_t, NeuronGene> neurons2 = genotype2.getNeurons();
    unordered_map<uint64_t, ConnectionGene> connections1 = genotype1.getConnections();
    unordered_map<uint64_t, ConnectionGene> connections2 = genotype2.getConnections();

    /* neurons of both genotypes should be the same */
    ASSERT_EQUAL(neurons1.size(), neurons2.size(), "not the same number of neurons");
    for (const auto &e : neurons1)
        ASSERT_EQUAL(neurons2.count(e.second.getID()), 1, "neuron not found");

    /* all connections sould be equal */
    ASSERT_EQUAL(connections1.size(), connections2.size(), "not the same number of connections");
    for (const auto &e : connections1)
        ASSERT_EQUAL(connections2.count(e.second.getID()), 1, "connection not found");

    vector<ConnectionGene> vConnections;
    for (const auto &e : connections1)
        vConnections.push_back(e.second);

    /* find a connection between a input and a output neuron */
    uint64_t randIndex = rand64(args) % vConnections.size();
    while (vConnections[randIndex].getInput().getType()  != NeuronType::INPUT ||
          vConnections[randIndex].getOutput().getType() != NeuronType::OUTPUT) {

        randIndex = rand64(args) % vConnections.size();
    }

    /* add a new neuron */
    genotype1.addNeuron(vConnections[randIndex]);
    genotype2.addNeuron(vConnections[randIndex]);

    /* recalculate the number of possible connections: 
     * 1 recurent connection
     * 1 Bias connection
     * a connection from each input
     * a connection to and from each output
     */
    nConnections += 1 + 1 + nInputs + 2 * nOutputs;

    /* create all posible connections */
    for (unsigned i = 0; i < nConnections * 100; i++) {
        genotype1.mutateInsertConnection();
        genotype2.mutateInsertConnection();
    }

    ASSERT_EQUAL(genotype1.getConnections().size(), 
                 nConnections,
                 "more than %lu connections: %lu",
                 nConnections,
                 genotype1.getConnections().size());
    
    ASSERT_EQUAL(genotype2.getConnections().size(), 
                 nConnections,
                 "more than %lu connections: %lu",
                 nConnections,
                 genotype2.getConnections().size());

    neurons1 = genotype1.getNeurons();
    neurons2 = genotype2.getNeurons();
    connections1 = genotype1.getConnections();
    connections2 = genotype2.getConnections();

    /* neurons of both genotypes should be the same */
    ASSERT_EQUAL(neurons1.size(), neurons2.size(), "not the same number of neurons");
    for (const auto &e : neurons1)
        ASSERT_EQUAL(neurons2.count(e.second.getID()), 1, "neuron not found");

    /* all connections sould be equal */
    ASSERT_EQUAL(connections1.size(), connections2.size(), "not the same number of connections");
    for (const auto &e : connections1)
        ASSERT_EQUAL(connections2.count(e.second.getID()), 1, "connection not found");

    return args;
}

TEST(basicSimilarityTest) {
    
    Genotype genotype1(2, 2);
    Genotype genotype2(2, 2);

    /* sort the neurons by input and output */
    unordered_map<uint64_t, NeuronGene> neuronsMap1 = genotype1.getNeurons();
    unordered_map<uint64_t, NeuronGene> neuronsMap2 = genotype2.getNeurons();
    vector<NeuronGene> inputs1, inputs2;
    vector<NeuronGene> outputs1, outputs2;
    
    for (const auto &e : neuronsMap1) {
        if (e.second.getType() == NeuronType::INPUT || 
            e.second.getType() == NeuronType::BIAS)
            inputs1.push_back(e.second);
        else if (e.second.getType() == NeuronType::OUTPUT)
            outputs1.push_back(e.second);
        else
            ASSERT_FAIL("network should not have hidden neurons");
    }

    for (const auto &e : neuronsMap2) {
        if (e.second.getType() == NeuronType::INPUT || 
            e.second.getType() == NeuronType::BIAS)
            inputs2.push_back(e.second);
        else if (e.second.getType() == NeuronType::OUTPUT)
            outputs2.push_back(e.second);
        else
            ASSERT_FAIL("network should not have hidden neurons");
    }

    ASSERT_EQUAL(inputs1.size(), 3, "more than 3 inputs");
    ASSERT_EQUAL(outputs1.size(), 2, "more than 2 outputs");
    ASSERT_EQUAL(inputs2.size(), 3, "more than 3 inputs");
    ASSERT_EQUAL(outputs2.size(), 2, "more than 2 outputs");

    ASSERT_EQUAL(genotype1.similarity(genotype2), 0, "genotyps should be equal");
    ASSERT_EQUAL(genotype2.similarity(genotype1), 0, "genotyps should be equal");

    genotype1.addConnection(inputs1[0], outputs1[1]);

    ASSERT_EQUAL(genotype1.similarity(genotype2), 1, "genotyps should have a similarity of 1");
    ASSERT_EQUAL(genotype2.similarity(genotype1), 1, "genotyps should have a similarity of 1");

    genotype2.addConnection(inputs2[1], outputs2[0]);

    ASSERT_EQUAL(genotype1.similarity(genotype2), 2, "genotyps should have a similarity of 2");
    ASSERT_EQUAL(genotype2.similarity(genotype1), 2, "genotyps should have a similarity of 2");

    genotype1.addConnection(inputs2[1], outputs2[0]);

    ASSERT_EQUAL(genotype1.similarity(genotype2), 1, "genotyps should have a similarity of 1");
    ASSERT_EQUAL(genotype2.similarity(genotype1), 1, "genotyps should have a similarity of 1");

    genotype2.addConnection(inputs1[0], outputs1[1]);

    ASSERT_EQUAL(genotype1.similarity(genotype2), 0, "genotyps should be equal");
    ASSERT_EQUAL(genotype2.similarity(genotype1), 0, "genotyps should be equal");

    return args;
}

TEST(basicRecombinationTest) {
    
    Genotype genotype1(2, 2);
    Genotype genotype2(2, 2);

    /* sort the neurons by input and output */
    unordered_map<uint64_t, NeuronGene> neuronsMap1 = genotype1.getNeurons();
    unordered_map<uint64_t, NeuronGene> neuronsMap2 = genotype2.getNeurons();
    vector<NeuronGene> inputs1, inputs2;
    vector<NeuronGene> outputs1, outputs2;
    
    for (const auto &e : neuronsMap1) {
        if (e.second.getType() == NeuronType::INPUT || 
            e.second.getType() == NeuronType::BIAS)
            inputs1.push_back(e.second);
        else if (e.second.getType() == NeuronType::OUTPUT)
            outputs1.push_back(e.second);
        else
            ASSERT_FAIL("network should not have hidden neurons");
    }

    for (const auto &e : neuronsMap2) {
        if (e.second.getType() == NeuronType::INPUT || 
            e.second.getType() == NeuronType::BIAS)
            inputs2.push_back(e.second);
        else if (e.second.getType() == NeuronType::OUTPUT)
            outputs2.push_back(e.second);
        else
            ASSERT_FAIL("network should not have hidden neurons");
    }

    ASSERT_EQUAL(inputs1.size(), 3, "more than 3 inputs");
    ASSERT_EQUAL(outputs1.size(), 2, "more than 2 outputs");
    ASSERT_EQUAL(inputs2.size(), 3, "more than 3 inputs");
    ASSERT_EQUAL(outputs2.size(), 2, "more than 2 outputs");

    ConnectionGene gene1 = genotype1.addConnection(inputs1[0], outputs1[1]);
    ConnectionGene gene2 = genotype1.addConnection(inputs2[1], outputs2[0]);
    ConnectionGene gene3 = genotype2.addConnection(inputs2[1], outputs2[0]);
    NeuronGene     gene4 = genotype2.addNeuron(gene3);

    Genotype child1 = Genotype(genotype1, genotype2);
    Genotype child2 = Genotype(genotype2, genotype1);

    ASSERT_EQUAL(child1.getConnections().count(gene1.getID()), 1, "child 1 shoudl have gene1");
    ASSERT_EQUAL(child1.getConnections().count(gene2.getID()), 1, "child 1 shoudl have gene2");
    ASSERT_EQUAL(child1.getConnections().count(gene3.getID()), 1, "child 1 shoudl have gene3");
    ASSERT_EQUAL(child1.getNeurons().count(gene4.getID()),     0, "child 1 shoudln't have gene4");

    ASSERT_EQUAL(child2.getConnections().count(gene1.getID()), 0, "child 2 shoudln't have gene1");
    ASSERT_EQUAL(child2.getConnections().count(gene2.getID()), 1, "child 2 shoudl have gene2");
    ASSERT_EQUAL(child2.getConnections().count(gene3.getID()), 1, "child 2 shoudl have gene3");
    ASSERT_EQUAL(child2.getNeurons().count(gene4.getID()),     1, "child 2 shoudl have gene4");

    return args;
}
