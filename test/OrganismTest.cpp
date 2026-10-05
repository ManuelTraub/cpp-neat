#include "../src/utils/utils.h"
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

TEST(basicSortingTest) {
    
    Opts opts;
    GeneHistory geneHistory(opts);
    vector<shared_ptr<Organism>> organisms;

    for (unsigned i = 0; i < 1000; i++) {
        organisms.push_back(make_shared<Organism>(0, 0, opts, geneHistory));
        organisms.back()->setFitness(rand_range_d64(args, 0, 100));
    }

    std::sort(organisms.begin(), organisms.end(), orderOrganisms);

    for (unsigned i = 1; i < organisms.size(); i++) {
        ASSERT_TRUE(organisms[i - 1]->getFitness() >= organisms[i]->getFitness(),
                    "organisms not sorted: %d:%F < %d:%F",
                    i - 1,
                    organisms[i - 1]->getFitness(),
                    i,
                    organisms[i]->getFitness());
    }

    return args;
}

TEST(advancedSortingTest) {

    /* speedup mutation for testing */
    Opts opts;
    opts.addNeuronMutationPropability(0.3);
    opts.addConnectionMutationPropability(0.5);
    GeneHistory geneHistory(opts);

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};
    
    vector<shared_ptr<Organism>> organisms;
    organisms.push_back(make_shared<Organism>(2, 1, opts, geneHistory));
    organisms.back()->calculateFittness(trainingInputs, trainingOutputs);


    for (unsigned i = 0; i < 1000; i++) {
        organisms.push_back(make_shared<Organism>(organisms.back()->clone()));
        organisms.back()->mutate();
        organisms.back()->calculateFittness(trainingInputs, trainingOutputs);
    }

    std::sort(organisms.begin(), organisms.end(), orderOrganisms);

    for (unsigned i = 1; i < organisms.size(); i++) {
        ASSERT_TRUE(organisms[i - 1]->getFitness() >= organisms[i]->getFitness(),
                    "organisms not sorted: %d:%F < %d:%F",
                    i - 1,
                    organisms[i - 1]->getFitness(),
                    i,
                    organisms[i]->getFitness());
    }

    return args;
}
