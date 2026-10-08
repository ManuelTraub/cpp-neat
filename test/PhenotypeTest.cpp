#include "../src/utils/utils.h"
#include "../src/Phenotype.h"
#include "TestAccess.h"

#include <vector>
#include <type_traits>

using std::vector;
using namespace NEAT;

static_assert(!std::is_copy_constructible<Phenotype>::value, "Phenotype must not be copyable");

#define static_t rand64_t
#define test_t rand64_t
#include "TestSuite.h"

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

TEST_N(asymmetricMappingSaveLoadCrossoverTest, 100) {

    Opts opts;
    opts.netActivationFunction(ActivationFunction::RELU);
    GeneHistory geneHistory(opts);
    Genotype genotype(2, 2, opts, geneHistory);

    const NeuronGene input0 = geneHistory.insertNeuron(NeuronType::INPUT, 0);
    const NeuronGene input1 = geneHistory.insertNeuron(NeuronType::INPUT, 1);
    const NeuronGene output0 = geneHistory.insertNeuron(NeuronType::OUTPUT, 0);
    const NeuronGene output1 = geneHistory.insertNeuron(NeuronType::OUTPUT, 1);
    const NeuronGene bias = geneHistory.insertNeuron(NeuronType::BIAS, 0);

    ConnectionGene &direct0 = TestAccess::addConnection(genotype, input0, output0);
    ConnectionGene &direct1 = TestAccess::addConnection(genotype, input1, output1);
    direct0.setWeight(0.0);
    direct1.setWeight(0.0);

    NeuronGene &hidden0 = TestAccess::addNeuron(genotype, direct0);
    NeuronGene &hidden1 = TestAccess::addNeuron(genotype, direct1);
    hidden0.setGradient(1.0);
    hidden1.setGradient(2.0);

    TestAccess::addConnection(genotype, input0, hidden0).setWeight(2.0);
    TestAccess::addConnection(genotype, input1, hidden1).setWeight(3.0);
    TestAccess::addConnection(genotype, hidden0, output0).setWeight(1.0);
    TestAccess::addConnection(genotype, hidden1, output0).setWeight(2.0);
    TestAccess::addConnection(genotype, hidden0, output1).setWeight(4.0);
    TestAccess::addConnection(genotype, hidden1, output1).setWeight(5.0);
    TestAccess::addConnection(genotype, bias, output0).setWeight(0.5);
    TestAccess::addConnection(genotype, bias, output1).setWeight(1.5);

    Phenotype original(genotype, opts);
    const vector<double> beforeSaving = original.run({0.25, 0.75});
    ASSERT_TRUE(absDiff(beforeSaving[0], output0.getGradient() * 10.0) < 1e-10,
                "first output mapping changed before saving");
    ASSERT_TRUE(absDiff(beforeSaving[1], output1.getGradient() * 26.0) < 1e-10,
                "second output mapping changed before saving");

    char fname[64];
    sprintf(fname, "temp%lu.gene", rand64(args));
    const int fd = open(fname, O_RDWR|O_CREAT|O_TRUNC, 00777);
    ASSERT_TRUE(fd >= 0, "failed to open file");
    ASSERT_TRUE(genotype.save(fd), "failed to save genotype");
    ASSERT_EQUAL(lseek(fd, 0, SEEK_SET), 0, "failed to rewind file");

    Genotype loadedGenotype(fd, opts, geneHistory);
    ASSERT_FALSE(close(fd), "failed to close file");
    ASSERT_FALSE(unlink(fname), "failed to delete file");

    Genotype crossedGenotype(genotype, genotype, opts, geneHistory);
    Phenotype loaded(loadedGenotype, opts);
    Phenotype crossed(crossedGenotype, opts);

    const vector<vector<double>> inputs = {{0, 0}, {1, 0}, {0, 1}, {0.25, 0.75}, {0.75, 0.25}};
    for (Phenotype *network : {&original, &loaded, &crossed}) {
        const vector<vector<Neuron *>> layers = network->getLayers();
        ASSERT_EQUAL(layers.size(), 3, "layer order changed");
        ASSERT_EQUAL(layers[0].size(), 2, "input layer size changed");
        ASSERT_EQUAL(layers[1].size(), 2, "hidden layer size changed");
        ASSERT_EQUAL(layers[2].size(), 2, "output layer size changed");
        ASSERT_EQUAL(layers[1][0]->getGradient(), 1.0, "first hidden neuron changed position");
        ASSERT_EQUAL(layers[1][1]->getGradient(), 2.0, "second hidden neuron changed position");

        for (const auto &input : inputs) {
            const vector<double> output = network->run(input);
            const double expected0 = output0.getGradient() * (2.0 * input[0] + 12.0 * input[1] + 0.5);
            const double expected1 = output1.getGradient() * (8.0 * input[0] + 30.0 * input[1] + 1.5);
            ASSERT_EQUAL(output.size(), 2, "output size changed");
            ASSERT_TRUE(absDiff(output[0], expected0) < 1e-10, "first output mapping changed");
            ASSERT_TRUE(absDiff(output[1], expected1) < 1e-10, "second output mapping changed");
        }
    }

    return args;
}

TEST(initNetZeroCyclesPreservesBiasTest) {

    for (unsigned numInputs : {0u, 1u}) {
        Opts opts;
        opts.allowRecurentConnections(true);
        GeneHistory geneHistory(opts);
        Genotype genotype(numInputs, 1, opts, geneHistory);

        const NeuronGene output = geneHistory.insertNeuron(NeuronType::OUTPUT, 0);
        const NeuronGene bias = geneHistory.insertNeuron(NeuronType::BIAS, 0);
        TestAccess::addConnection(genotype, bias, output).setWeight(0.75);
        TestAccess::addConnection(genotype, output, output).setWeight(0.25);

        Phenotype phenotype(genotype, opts);
        const vector<double> inputs(numInputs, 0.5);
        const double expected = 1.0 / (1.0 + exp(-output.getGradient() * 0.75));
        const double initialOutput = phenotype.run(inputs)[0];
        ASSERT_TRUE(absDiff(initialOutput, expected) < 1e-12, "initial bias output changed");
        ASSERT_TRUE(phenotype.run(inputs)[0] > expected, "recurrent state was not retained");

        phenotype.initNet(0);
        for (const auto &layer : phenotype.getLayers())
            for (const auto &neuron : layer)
                ASSERT_EQUAL(neuron->getValue(), 0.0, "initNet(0) did not clear neuron state");

        const double resetOutput = phenotype.run(vector<double>(numInputs, 0.0))[0];
        ASSERT_TRUE(absDiff(resetOutput, expected) < 1e-12,
                    "initNet(0) changed constant bias or retained recurrent state");
    }

    return args;
}

TEST(initNetDefaultCyclesWarmupTest) {

    Opts opts;
    opts.netActivationFunction(ActivationFunction::RELU);
    opts.allowRecurentConnections(true);
    GeneHistory geneHistory(opts);
    Genotype genotype(0, 1, opts, geneHistory);

    const NeuronGene output = geneHistory.insertNeuron(NeuronType::OUTPUT, 0);
    const NeuronGene bias = geneHistory.insertNeuron(NeuronType::BIAS, 0);
    /* each pass adds one to the recurrent output */
    TestAccess::addConnection(genotype, bias, output).setWeight(1.0 / output.getGradient());
    TestAccess::addConnection(genotype, output, output).setWeight(1.0 / output.getGradient());

    Phenotype phenotype(genotype, opts);
    phenotype.run({});
    phenotype.initNet();

    const double warmedOutput = phenotype.getLayers().back()[0]->getValue();
    ASSERT_TRUE(absDiff(warmedOutput, 200.0) < 1e-10,
                "default initNet did not reset and warm up for 200 cycles");
    const double nextOutput = phenotype.run({})[0];
    ASSERT_TRUE(absDiff(nextOutput, 201.0) < 1e-10,
                "default initNet did not preserve warmed recurrent state");

    return args;
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

    ConnectionGene &inCon1  = TestAccess::addConnection(genotype, inputs[0], outputs[0]);
    ConnectionGene &inCon2  = TestAccess::addConnection(genotype, inputs[1], outputs[0]);
    ConnectionGene &biasCon = TestAccess::addConnection(genotype, *bias, outputs[0]);

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

    ConnectionGene &inCon1  = TestAccess::addConnection(genotype, inputs[0], outputs[0]);
    ConnectionGene &inCon2  = TestAccess::addConnection(genotype, inputs[1], outputs[0]);
    ConnectionGene &biasCon = TestAccess::addConnection(genotype, *bias, outputs[0]);

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
