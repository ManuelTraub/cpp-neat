#include "../src/Gene.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <vector>

using std::vector;
using namespace NEAT;

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

class LoadableGene : public Gene {
    public:
        using Gene::loadBase;
};

TEST(truncatedHeaderTest) {

    const uint64_t header[] = {0, GenerationCounter::get()};
    char fname[64];
    sprintf(fname, "temp%lu.gene", rand64(args));

    for (unsigned length = 0; length < sizeof(header); length++) {
        const int fd = open(fname, O_RDWR|O_CREAT|O_TRUNC, 00777);
        ASSERT_TRUE(fd >= 0, "failed to open file");
        ASSERT_EQUAL(write(fd, header, length), length, "failed to write truncated header");
        ASSERT_EQUAL(lseek(fd, 0, SEEK_SET), 0, "failed to rewind file");

        LoadableGene gene;
        const bool loaded = gene.loadBase(fd);
        ASSERT_EQUAL(lseek(fd, 0, SEEK_SET), 0, "failed to rewind file");

        bool rejected = false;
        try {
            NeuronGene neuron(fd);
        } catch (const char *) {
            rejected = true;
        }

        ASSERT_FALSE(close(fd), "failed to close file");
        ASSERT_FALSE(loaded, "truncated header accepted at %u bytes", length);
        ASSERT_TRUE(rejected, "truncated neuron header accepted at %u bytes", length);
    }

    ASSERT_FALSE(unlink(fname), "failed to delete file");
    return args;
}

TEST_N(basicSaveLoadTest, 10000) {
    

    NeuronGene input(NeuronType::INPUT, ActivationFunction::SIGMOID, rand64(args));
    NeuronGene output(NeuronType::OUTPUT, ActivationFunction::TANH, rand64(args));
    NeuronGene bias(NeuronType::BIAS, ActivationFunction::SIGMOID, rand64(args));

    ConnectionGene connection(input, output);
    NeuronGene hidden = connection.split(ActivationFunction::RELU);

    char fname[64];
    sprintf(fname, "temp%lu.gene", rand64(args));
    const int fd = open(fname, O_WRONLY|O_CREAT|O_TRUNC, 00777);

    ASSERT_TRUE(fd > 0, "failed to open file for writing");

    ASSERT_TRUE(input.save(fd), "failed to write gene");
    ASSERT_TRUE(output.save(fd), "failed to write gene");
    ASSERT_TRUE(bias.save(fd), "failed to write gene");
    ASSERT_TRUE(connection.save(fd), "failed to write gene");
    ASSERT_TRUE(hidden.save(fd), "failed to write gene");
    ASSERT_FALSE(close(fd), "failed to close file");

    const int fd2 = open(fname, O_RDONLY);
    ASSERT_TRUE(fd2 > 0, "failed to open file for reading");



    NeuronGene input2(fd2);
    NeuronGene output2(fd2);
    NeuronGene bias2(fd2);

    ConnectionGene connection2(fd2);
    NeuronGene hidden2(fd2);

    ASSERT_FALSE(close(fd2), "failed to close file");

    ASSERT_EQUAL(input.getID(),         input2.getID(),         "ids not equal");
    ASSERT_EQUAL(input.getGeneration(), input2.getGeneration(), "generation not equal");
    ASSERT_EQUAL(input.getType(),       input2.getType(),       "types not equal");
    ASSERT_EQUAL(input.getLayerID(),    input2.getLayerID(),    "layerIDs not equal");
    ASSERT_EQUAL(input.getGradient(),   input2.getGradient(),   "gradients not equal");
    ASSERT_EQUAL(input.getFunction(),   input2.getFunction(),   "activation functions not equal");

    ASSERT_EQUAL(output.getID(),         output2.getID(),         "ids not equal");
    ASSERT_EQUAL(output.getGeneration(), output2.getGeneration(), "generation not equal");
    ASSERT_EQUAL(output.getType(),       output2.getType(),       "types not equal");
    ASSERT_EQUAL(output.getLayerID(),    output2.getLayerID(),    "layerIDs not equal");
    ASSERT_EQUAL(output.getGradient(),   output2.getGradient(),   "gradients not equal");
    ASSERT_EQUAL(output.getFunction(),   output2.getFunction(),   "activation functions not equal");

    ASSERT_EQUAL(bias.getID(),         bias2.getID(),         "ids not equal");
    ASSERT_EQUAL(bias.getGeneration(), bias2.getGeneration(), "generation not equal");
    ASSERT_EQUAL(bias.getType(),       bias2.getType(),       "types not equal");
    ASSERT_EQUAL(bias.getLayerID(),    bias2.getLayerID(),    "layerIDs not equal");
    ASSERT_EQUAL(bias.getGradient(),   bias2.getGradient(),   "gradients not equal");
    ASSERT_EQUAL(bias.getFunction(),   bias2.getFunction(),   "activation functions not equal");

    ASSERT_EQUAL(hidden.getID(),         hidden2.getID(),         "ids not equal");
    ASSERT_EQUAL(hidden.getGeneration(), hidden2.getGeneration(), "generation not equal");
    ASSERT_EQUAL(hidden.getType(),       hidden2.getType(),       "types not equal");
    ASSERT_EQUAL(hidden.getLayerID(),    hidden2.getLayerID(),    "layerIDs not equal");
    ASSERT_EQUAL(hidden.getGradient(),   hidden2.getGradient(),   "gradients not equal");
    ASSERT_EQUAL(hidden.getFunction(),   hidden2.getFunction(),   "activation functions not equal");

    ASSERT_EQUAL(connection.getID(),                     connection2.getID(),                     "ids not equal");
    ASSERT_EQUAL(connection.getGeneration(),             connection2.getGeneration(),             "generation not equal");
    ASSERT_EQUAL(connection.getInput().getID(),          connection2.getInput().getID(),          "ids not equal");
    ASSERT_EQUAL(connection.getInput().getGeneration(),  connection2.getInput().getGeneration(),  "generation not equal");
    ASSERT_EQUAL(connection.getInput().getType(),        connection2.getInput().getType(),        "types not equal");
    ASSERT_EQUAL(connection.getInput().getLayerID(),     connection2.getInput().getLayerID(),     "layerIDs not equal");
    ASSERT_EQUAL(connection.getInput().getGradient(),    connection2.getInput().getGradient(),    "gradients not equal");
    ASSERT_EQUAL(connection.getOutput().getID(),         connection2.getOutput().getID(),         "ids not equal");
    ASSERT_EQUAL(connection.getOutput().getGeneration(), connection2.getOutput().getGeneration(), "generation not equal");
    ASSERT_EQUAL(connection.getOutput().getType(),       connection2.getOutput().getType(),       "types not equal");
    ASSERT_EQUAL(connection.getOutput().getLayerID(),    connection2.getOutput().getLayerID(),    "layerIDs not equal");
    ASSERT_EQUAL(connection.getOutput().getGradient(),   connection2.getOutput().getGradient(),   "gradients not equal");
    ASSERT_EQUAL(connection.getWeight(),                 connection2.getWeight(),                 "weights not equal");
    ASSERT_EQUAL(connection.isEnabled(),                 connection2.isEnabled(),                 "enabled not equal");

    ASSERT_FALSE(unlink(fname), "failed to delete file");
    return args;
}
