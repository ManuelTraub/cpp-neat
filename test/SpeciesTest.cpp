#include "../src/utils.h"
#include "../src/Species.h"

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
    
    vector<shared_ptr<Species>> species;

    for (unsigned i = 0; i < 1000; i++) {
        species.push_back(make_shared<Species>());
        species.back()->setBestFitness(rand_range_d64(args, 0, 100));
    }

    std::sort(species.begin(), species.end(), SpeciesComparator());

    for (unsigned i = 1; i < species.size(); i++) {
        ASSERT_TRUE(species[i - 1]->getBestFitness() < species[i]->getBestFitness(),
                    "species not sorted: %d:%F < %d:%F",
                    i - 1,
                    species[i - 1]->getBestFitness(),
                    i,
                    species[i]->getBestFitness());
    }

    return args;
}
