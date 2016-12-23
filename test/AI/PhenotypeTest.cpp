#include "../../src/utils/utils.h"
#include "../../src/AI/Phenotype.h"

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
    Genotype genotype(2, 1, opts, geneHistory);

    /* create all posible connections */
    for (unsigned i = 0; i < 1000; i++)
        genotype.mutateInsertConnection();

    Phenotype phenotype(genotype, opts);

    ASSERT_EQUAL(phenotype.getLayers().size(), 2,
                 "expected 2 layers within phenotype: %lu",
                 phenotype.getLayers().size());

    ASSERT_EQUAL(phenotype.getLayers()[0].size(), 2,
                 "expected 1. layer to have 2 neurons: %lu",
                 phenotype.getLayers()[0].size());

    ASSERT_EQUAL(phenotype.getLayers()[1].size(), 1,
                 "expected 2. layer to have 1 neuron: %lu",
                 phenotype.getLayers()[1].size());

    phenotype.saveAsSVG("Phenotype.svg");

    return args;
}

TEST_N(advancedTest, 100) {
    
    Opts opts;
    GeneHistory geneHistory(opts);
    Genotype genotype(5, 5, opts, geneHistory);

    for (unsigned n = 0; n < 20; n++) {

        /* create connections */
        for (unsigned i = 0; i < 100; i++)
            genotype.mutateInsertConnection();

        /* insert a new neuron */
        while (!genotype.mutateInsertNeuron());
    }

    Phenotype phenotype(genotype, opts);
    phenotype.saveAsSVG("Phenotype2.svg");

    return args;
}

TEST(basicBinaryANDTest) {

    Opts opts;
    GeneHistory geneHistory(opts);
    Genotype genotype(2, 1, opts, geneHistory);

    unordered_map<uint64_t, NeuronGene> neuronsMap = genotype.getNeurons();
    vector<NeuronGene> inputs;
    vector<NeuronGene> outputs;
    const NeuronGene *bias = NULL;
    
    for (const auto &e : neuronsMap) {
        if (e.second.getType() == NeuronType::INPUT)
            inputs.push_back(e.second);
        else if (e.second.getType() == NeuronType::BIAS)
            bias = &e.second;
        else if (e.second.getType() == NeuronType::OUTPUT)
            outputs.push_back(e.second);
        else
            ASSERT_FAIL("network should not have hidden neurons");
    }

    ASSERT_EQUAL(inputs.size(), 2, "there should be 2 inputs");
    ASSERT_EQUAL(outputs.size(), 1, "there should be 1 output");

    ConnectionGene &inCon1  = genotype.addConnection(inputs[0], outputs[0]);
    ConnectionGene &inCon2  = genotype.addConnection(inputs[1], outputs[0]);
    ConnectionGene &biasCon = genotype.addConnection(*bias, outputs[0]);

    inCon1.setWeight(0.5);
    inCon2.setWeight(0.5);
    biasCon.setWeight(-0.8);

    Phenotype phenotype(genotype, opts);
    phenotype.saveAsSVG("Phenotype-binary-AND.svg");

    ASSERT_TRUE(phenotype.run({0,0})[0] < 0.5, "AND(0,0) = 0: %F", phenotype.run({0,0})[0]);
    ASSERT_TRUE(phenotype.run({0,1})[0] < 0.5, "AND(0,1) = 0: %F", phenotype.run({0,1})[0]);
    ASSERT_TRUE(phenotype.run({1,0})[0] < 0.5, "AND(1,0) = 0: %F", phenotype.run({1,0})[0]);
    ASSERT_TRUE(phenotype.run({1,1})[0] > 0.5, "AND(1,1) = 1: %F", phenotype.run({1,1})[0]);

    return args;
}

TEST_N(advancedBinaryANDTest, 1000) {

    Opts opts;
    GeneHistory geneHistory(opts);
    Genotype genotype(2, 1, opts, geneHistory);

    unordered_map<uint64_t, NeuronGene> neuronsMap = genotype.getNeurons();
    vector<NeuronGene> inputs;
    vector<NeuronGene> outputs;
    const NeuronGene *bias = NULL;
    
    for (const auto &e : neuronsMap) {
        if (e.second.getType() == NeuronType::INPUT)
            inputs.push_back(e.second);
        else if (e.second.getType() == NeuronType::BIAS)
            bias = &e.second;
        else if (e.second.getType() == NeuronType::OUTPUT)
            outputs.push_back(e.second);
        else
            ASSERT_FAIL("network should not have hidden neurons");
    }

    ASSERT_EQUAL(inputs.size(), 2, "there should be 2 inputs");
    ASSERT_EQUAL(outputs.size(), 1, "there should be 1 output");

    ConnectionGene &inCon1  = genotype.addConnection(inputs[0], outputs[0]);
    ConnectionGene &inCon2  = genotype.addConnection(inputs[1], outputs[0]);
    ConnectionGene &biasCon = genotype.addConnection(*bias, outputs[0]);

    double weight1 = inCon1.getWeight(), 
           weight2 = inCon2.getWeight(), 
           weight3 = biasCon.getWeight();

    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};


    double minError = UINT64_MAX;
    while (minError > 0.01) {
        genotype.mutateConnections();
        Phenotype phenotype(genotype, opts);
        double curError = phenotype.calculateError(trainingInputs, trainingOutputs);

        if (curError < minError) {
            minError = curError;
            weight1 = inCon1.getWeight(), 
            weight2 = inCon2.getWeight(), 
            weight3 = biasCon.getWeight();
        } else {
            inCon1.setWeight(weight1);
            inCon2.setWeight(weight2);
            biasCon.setWeight(weight3);
        }
        TEST_MSGD("minError: %F, cur %F", minError, curError);
    }


    Phenotype phenotype(genotype, opts);
    phenotype.saveAsSVG("Phenotype-binary-AND2.svg");

    ASSERT_TRUE(phenotype.run({0,0})[0] < 0.5, "AND(0,0) = 0: %F", phenotype.run({0,0})[0]);
    ASSERT_TRUE(phenotype.run({0,1})[0] < 0.5, "AND(0,1) = 0: %F", phenotype.run({0,1})[0]);
    ASSERT_TRUE(phenotype.run({1,0})[0] < 0.5, "AND(1,0) = 0: %F", phenotype.run({1,0})[0]);
    ASSERT_TRUE(phenotype.run({1,1})[0] > 0.5, "AND(1,1) = 1: %F", phenotype.run({1,1})[0]);

    TEST_MSGD("AND(0,0) = 0: %F", phenotype.run({0,0})[0]);
    TEST_MSGD("AND(0,1) = 0: %F", phenotype.run({0,1})[0]);
    TEST_MSGD("AND(1,0) = 0: %F", phenotype.run({1,0})[0]);
    TEST_MSGD("AND(1,1) = 1: %F", phenotype.run({1,1})[0]);

    TEST_MSGD("Input Connection 1: %F", weight1);
    TEST_MSGD("Input Connection 2: %F", weight2);
    TEST_MSGD("Bias Connection:    %F", weight3);

    return args;
}

TEST(phenotypeGenerationSpeedTest) {

    Opts opts;
    GeneHistory geneHistory(opts);
    Genotype genotype(10, 10, opts, geneHistory);

    for (unsigned i = 0; i < 100; i++)
        genotype.mutateInsertNeuron();

    for (unsigned i = 0; i < 1000; i++)
        genotype.mutateInsertConnection();
    
    uint64_t time = gettime_usec();
    for (unsigned i = 0; i < 1000; i++)
        Phenotype phenotype(genotype, opts);

    TEST_MSGD("Phenotype generation time: %.10F", ((double) (gettime_usec() - time)) / 1000000.0);

    return args;
}
