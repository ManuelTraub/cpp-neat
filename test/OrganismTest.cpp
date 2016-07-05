#include "../src/utils.h"
#include "../src/Organism.h"
#include "../src/Opts.h"

#include <vector>
#include <memory>
#include <algorithm>

using std::shared_ptr;
using std::make_shared;
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

TEST(basicSortingTest) {
    
    vector<shared_ptr<Organism>> organisms;

    for (unsigned i = 0; i < 1000; i++) {
        organisms.push_back(make_shared<Organism>(0, 0));
        organisms.back()->setOriginalFitness(rand_range_d64(args, 0, 100));
        organisms.back()->setAdjustedFitness(organisms.back()->getOriginalFitness() / 10);
    }

    std::sort(organisms.begin(), organisms.end(), OrganismComparator());

    for (unsigned i = 1; i < organisms.size(); i++) {
        ASSERT_TRUE(organisms[i - 1]->getAdjustedFitness() >= organisms[i]->getAdjustedFitness(),
                    "organisms not sorted: %d:%F < %d:%F",
                    i - 1,
                    organisms[i - 1]->getAdjustedFitness(),
                    i,
                    organisms[i]->getAdjustedFitness());
    }

    return args;
}

TEST(advancedSortingTest) {

    /* speedup mutation for testing */
    Opts::getInstance()._addNeuronMutationPropability = 0.3;
    Opts::getInstance()._addConnectionMutationPropability = 0.5;

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};
    
    vector<shared_ptr<Organism>> organisms;
    organisms.push_back(make_shared<Organism>(2, 1));
    organisms.back()->calculateFittness(trainingInputs, trainingOutputs, 10, 15, 2);


    for (unsigned i = 0; i < 1000; i++) {
        organisms.push_back(make_shared<Organism>(*organisms.back()));
        organisms.back()->calculateFittness(trainingInputs, trainingOutputs, 10, 15, 2);
    }

    std::sort(organisms.begin(), organisms.end(), OrganismComparator());

    for (unsigned i = 1; i < organisms.size(); i++) {
        ASSERT_TRUE(organisms[i - 1]->getAdjustedFitness() >= organisms[i]->getAdjustedFitness(),
                    "organisms not sorted: %d:%F < %d:%F",
                    i - 1,
                    organisms[i - 1]->getAdjustedFitness(),
                    i,
                    organisms[i]->getAdjustedFitness());
    }

    return args;
}
