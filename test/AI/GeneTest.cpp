#include "../../src/AI/Gene.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

TEST_N(basicSaveLoadTest, 10000) {
    

    NeuronGene input(NeuronType::INPUT, rand64(args));
    NeuronGene output(NeuronType::OUTPUT, rand64(args));
    NeuronGene bias(NeuronType::BIAS, rand64(args));

    ConnectionGene connection(input, output);
    NeuronGene hidden = connection.split();

    StructureGene tissue(TissueType::TISSUE, rand64(args), rand64(args), rand64(args));
    StructureGene neuron(TissueType::TISSUE, rand64(args), rand64(args), rand64(args));
    StructureGene sensor(TissueType::TISSUE, rand64(args), rand64(args), rand64(args));
    StructureGene muscle(TissueType::TISSUE, rand64(args), rand64(args), rand64(args));

    for (unsigned i = 0; i < rand64(args) % 10; i++);
        tissue.connectToGene(rand64(args));

    for (unsigned i = 0; i < rand64(args) % 10; i++);
        neuron.connectToGene(rand64(args));

    for (unsigned i = 0; i < rand64(args) % 10; i++);
        sensor.connectToGene(rand64(args));

    for (unsigned i = 0; i < rand64(args) % 10; i++);
        muscle.connectToGene(rand64(args));

    char fname[64];
    sprintf(fname, "temp%lu.gene", rand64(args));
    const int fd = open(fname, O_WRONLY|O_CREAT|O_TRUNC, 00777);

    ASSERT_TRUE(fd > 0, "failed to open file for writing");

    ASSERT_TRUE(input.save(fd), "failed to write gene");
    ASSERT_TRUE(output.save(fd), "failed to write gene");
    ASSERT_TRUE(bias.save(fd), "failed to write gene");
    ASSERT_TRUE(connection.save(fd), "failed to write gene");
    ASSERT_TRUE(hidden.save(fd), "failed to write gene");
    ASSERT_TRUE(tissue.save(fd), "failed to write gene");
    ASSERT_TRUE(neuron.save(fd), "failed to write gene");
    ASSERT_TRUE(sensor.save(fd), "failed to write gene");
    ASSERT_TRUE(muscle.save(fd), "failed to write gene");

    ASSERT_FALSE(close(fd), "failed to close file");

    const int fd2 = open(fname, O_RDONLY);
    ASSERT_TRUE(fd2 > 0, "failed to open file for reading");



    NeuronGene input2(fd2);
    NeuronGene output2(fd2);
    NeuronGene bias2(fd2);

    ConnectionGene connection2(fd2);
    NeuronGene hidden2(fd2);

    StructureGene tissue2(fd2);
    StructureGene neuron2(fd2);
    StructureGene sensor2(fd2);
    StructureGene muscle2(fd2);

    close(fd2);

    ASSERT_EQUAL(input.getID(),         input2.getID(),         "ids not equal");
    ASSERT_EQUAL(input.getGeneration(), input2.getGeneration(), "generation not equal");
    ASSERT_EQUAL(input.getType(),       input2.getType(),       "types not equal");
    ASSERT_EQUAL(input.getLayerID(),    input2.getLayerID(),    "layerIDs not equal");
    ASSERT_EQUAL(input.getGradient(),   input2.getGradient(),   "gradients not equal");

    ASSERT_EQUAL(output.getID(),         output2.getID(),         "ids not equal");
    ASSERT_EQUAL(output.getGeneration(), output2.getGeneration(), "generation not equal");
    ASSERT_EQUAL(output.getType(),       output2.getType(),       "types not equal");
    ASSERT_EQUAL(output.getLayerID(),    output2.getLayerID(),    "layerIDs not equal");
    ASSERT_EQUAL(output.getGradient(),   output2.getGradient(),   "gradients not equal");

    ASSERT_EQUAL(bias.getID(),         bias2.getID(),         "ids not equal");
    ASSERT_EQUAL(bias.getGeneration(), bias2.getGeneration(), "generation not equal");
    ASSERT_EQUAL(bias.getType(),       bias2.getType(),       "types not equal");
    ASSERT_EQUAL(bias.getLayerID(),    bias2.getLayerID(),    "layerIDs not equal");
    ASSERT_EQUAL(bias.getGradient(),   bias2.getGradient(),   "gradients not equal");

    ASSERT_EQUAL(hidden.getID(),         hidden2.getID(),         "ids not equal");
    ASSERT_EQUAL(hidden.getGeneration(), hidden2.getGeneration(), "generation not equal");
    ASSERT_EQUAL(hidden.getType(),       hidden2.getType(),       "types not equal");
    ASSERT_EQUAL(hidden.getLayerID(),    hidden2.getLayerID(),    "layerIDs not equal");
    ASSERT_EQUAL(hidden.getGradient(),   hidden2.getGradient(),   "gradients not equal");

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

    ASSERT_EQUAL(tissue.getID(),                 tissue2.getID(),                 "ids not equal");
    ASSERT_EQUAL(tissue.getGeneration(),         tissue2.getGeneration(),         "generation not equal");
    ASSERT_EQUAL(tissue.getType(),               tissue2.getType(),               "types not equal");
    ASSERT_EQUAL(tissue.getX(),                  tissue2.getX(),                  "x not equal");
    ASSERT_EQUAL(tissue.getY(),                  tissue2.getY(),                  "y not equal");
    ASSERT_EQUAL(tissue.getZ(),                  tissue2.getZ(),                  "z not equal");
    ASSERT_EQUAL(tissue.getConnections().size(), tissue2.getConnections().size(), "getConnections() size not equal");
    for (unsigned i = 0; i < tissue.getConnections().size(); i++)
        ASSERT_EQUAL(tissue.getConnections()[i], tissue2.getConnections()[i],     "getConnections() %u not equal", i);

    ASSERT_EQUAL(neuron.getID(),                 neuron2.getID(),                 "ids not equal");
    ASSERT_EQUAL(neuron.getGeneration(),         neuron2.getGeneration(),         "generation not equal");
    ASSERT_EQUAL(neuron.getType(),               neuron2.getType(),               "types not equal");
    ASSERT_EQUAL(neuron.getX(),                  neuron2.getX(),                  "x not equal");
    ASSERT_EQUAL(neuron.getY(),                  neuron2.getY(),                  "y not equal");
    ASSERT_EQUAL(neuron.getZ(),                  neuron2.getZ(),                  "z not equal");
    ASSERT_EQUAL(neuron.getConnections().size(), neuron2.getConnections().size(), "getConnections() size not equal");
    for (unsigned i = 0; i < neuron.getConnections().size(); i++)
        ASSERT_EQUAL(neuron.getConnections()[i], neuron2.getConnections()[i],     "getConnections() %u not equal", i);

    ASSERT_EQUAL(sensor.getID(),                 sensor2.getID(),                 "ids not equal");
    ASSERT_EQUAL(sensor.getGeneration(),         sensor2.getGeneration(),         "generation not equal");
    ASSERT_EQUAL(sensor.getType(),               sensor2.getType(),               "types not equal");
    ASSERT_EQUAL(sensor.getX(),                  sensor2.getX(),                  "x not equal");
    ASSERT_EQUAL(sensor.getY(),                  sensor2.getY(),                  "y not equal");
    ASSERT_EQUAL(sensor.getZ(),                  sensor2.getZ(),                  "z not equal");
    ASSERT_EQUAL(sensor.getConnections().size(), sensor2.getConnections().size(), "getConnections() size not equal");
    for (unsigned i = 0; i < sensor.getConnections().size(); i++)
        ASSERT_EQUAL(sensor.getConnections()[i], sensor2.getConnections()[i],     "getConnections() %u not equal", i);

    ASSERT_EQUAL(muscle.getID(),                 muscle2.getID(),                 "ids not equal");
    ASSERT_EQUAL(muscle.getGeneration(),         muscle2.getGeneration(),         "generation not equal");
    ASSERT_EQUAL(muscle.getType(),               muscle2.getType(),               "types not equal");
    ASSERT_EQUAL(muscle.getX(),                  muscle2.getX(),                  "x not equal");
    ASSERT_EQUAL(muscle.getY(),                  muscle2.getY(),                  "y not equal");
    ASSERT_EQUAL(muscle.getZ(),                  muscle2.getZ(),                  "z not equal");
    ASSERT_EQUAL(muscle.getConnections().size(), muscle2.getConnections().size(), "getConnections() size not equal");
    for (unsigned i = 0; i < muscle.getConnections().size(); i++)
        ASSERT_EQUAL(muscle.getConnections()[i], muscle2.getConnections()[i],     "getConnections() %u not equal", i);

    ASSERT_FALSE(unlink(fname), "failed to delete file");
    return args;
}
