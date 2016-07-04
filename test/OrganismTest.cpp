#include "../src/utils.h"
#include "../src/Organism.h"

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
        ASSERT_TRUE(organisms[i - 1]->getAdjustedFitness() > organisms[i]->getAdjustedFitness(),
                    "organisms not sorted: %d:%F < %d:%F",
                    i - 1,
                    organisms[i - 1]->getAdjustedFitness(),
                    i,
                    organisms[i]->getAdjustedFitness());
    }

    return args;
}
