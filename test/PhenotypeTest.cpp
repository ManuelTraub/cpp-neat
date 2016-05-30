#include "../src/utils.h"
#include "../src/Phenotype.h"

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
    
    Genotype genotype(2, 1);

    /* create all posible connections */
    for (unsigned i = 0; i < 1000; i++)
        genotype.mutateInsertConnection();

    Phenotype phenotype(&genotype);

    ASSERT_EQUAL(phenotype.getLayers().size(), 2,
                 "expected 2 layers within phenotype: %lu",
                 phenotype.getLayers().size());

    ASSERT_EQUAL(phenotype.getLayers()[0].size(), 3,
                 "expected 1. layer to have 3 neurons: %lu",
                 phenotype.getLayers()[0].size());

    ASSERT_EQUAL(phenotype.getLayers()[1].size(), 1,
                 "expected 2. layer to have 1 neuron: %lu",
                 phenotype.getLayers()[1].size());

    phenotype.saveAsSVG("Phenotype.svg");

    return args;
}

TEST_N(advancedTest, 100) {
    
    Genotype genotype(5, 5);

    for (unsigned n = 0; n < 20; n++) {

        /* create connections */
        for (unsigned i = 0; i < 100; i++)
            genotype.mutateInsertConnection();

        /* insert a new neuron */
        while (!genotype.mutateInsertNeuron());
    }

    Phenotype phenotype(&genotype);
    phenotype.saveAsSVG("Phenotype2.svg");

    return args;
}
