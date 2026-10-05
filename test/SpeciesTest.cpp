#include "../src/utils/utils.h"
#include "../src/Species.h"
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


    /* speedup mutation for testing */
    Opts opts;
    opts.addNeuronMutationPropability(0.3);
    opts.addConnectionMutationPropability(0.5);
    GeneHistory geneHistory(opts);

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};
    
    vector<shared_ptr<Species>> species;

    vector<shared_ptr<Organism>> organisms;
    organisms.push_back(make_shared<Organism>(2, 1, opts, geneHistory));
    organisms.back()->calculateFittness(trainingInputs, trainingOutputs);

    for (unsigned i = 0; i < 100; i++) {
        species.push_back(make_shared<Species>(opts, geneHistory));
        unsigned nOrganisms = rand_range64(args, 1, 15);
        for (unsigned n = 0; n < nOrganisms; n++) {
            organisms.push_back(make_shared<Organism>(organisms.back()->clone()));
            organisms.back()->mutate();
            species.back()->add(organisms.back());
        }
        species.back()->calculateFittness(trainingInputs, trainingOutputs);
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


    std::sort(species.begin(), species.end(), orderSpecies);

    for (unsigned i = 1; i < species.size(); i++) {
        ASSERT_TRUE(species[i - 1]->getBestFitness() >=
                    species[i]->getBestFitness(),
                    "species not sorted: %d:%F > %d:%F",
                    i - 1,
                    species[i - 1]->getBestFitness(),
                    i,
                    species[i]->getBestFitness());
    }

    return args;
}

TEST(offspringTest) {

    /* speedup mutation for testing */
    Opts opts;
    opts.addNeuronMutationPropability(0.3);
    opts.addConnectionMutationPropability(0.5);
    GeneHistory geneHistory(opts);

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};
    
    vector<shared_ptr<Species>> species;

    vector<shared_ptr<Organism>> organisms;
    organisms.push_back(make_shared<Organism>(2, 1, opts, geneHistory));
    organisms.back()->calculateFittness(trainingInputs, trainingOutputs);

    for (unsigned i = 0; i < 100; i++) {
        species.push_back(make_shared<Species>(opts, geneHistory));
        unsigned nOrganisms = rand_range64(args, 1, 15);
        for (unsigned n = 0; n < nOrganisms; n++) {
            organisms.push_back(make_shared<Organism>(organisms.back()->clone()));
            organisms.back()->mutate();
            species.back()->add(organisms.back());
        }
        species.back()->calculateFittness(trainingInputs, trainingOutputs);
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


    std::sort(species.begin(), species.end(), orderSpecies);

    for (unsigned i = 1; i < species.size(); i++) {
        ASSERT_TRUE(species[i - 1]->getBestFitness() >=
                    species[i]->getBestFitness(),
                    "species not sorted: %d:%F > %d:%F",
                    i - 1,
                    species[i - 1]->getBestFitness(),
                    i,
                    species[i]->getBestFitness());
    }

    for (auto &s : species)
        ASSERT_EQUAL(s->createOffsprings(10).size(), 10);

    return args;
}
