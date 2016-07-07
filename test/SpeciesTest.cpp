#include "../src/utils.h"
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
        species.back()->setAverageAdjustedFitness(rand_range_d64(args, 0, 100));
    }

    std::sort(species.begin(), species.end(), SpeciesComparator());

    for (unsigned i = 1; i < species.size(); i++) {
        ASSERT_TRUE(species[i - 1]->getAverageAdjustedFitness() >=
                    species[i]->getAverageAdjustedFitness(),
                    "species not sorted: %d:%F > %d:%F",
                    i - 1,
                    species[i - 1]->getAverageAdjustedFitness(),
                    i,
                    species[i]->getAverageAdjustedFitness());
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
    
    vector<shared_ptr<Species>> species;

    vector<shared_ptr<Organism>> organisms;
    organisms.push_back(make_shared<Organism>(2, 1));
    organisms.back()->calculateFittness(trainingInputs, trainingOutputs, 10, 15, 2);

    for (unsigned i = 0; i < 100; i++) {
        species.push_back(make_shared<Species>());
        unsigned nOrganisms = rand_range64(args, 1, 15);
        for (unsigned n = 0; n < nOrganisms; n++) {
            organisms.push_back(make_shared<Organism>(organisms.back()->clone()));
            organisms.back()->mutate();
            species.back()->add(organisms.back());
        }
        species.back()->calculateFittness(trainingInputs, trainingOutputs);
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


    std::sort(species.begin(), species.end(), SpeciesComparator());

    for (unsigned i = 1; i < species.size(); i++) {
        ASSERT_TRUE(species[i - 1]->getAverageAdjustedFitness() >=
                    species[i]->getAverageAdjustedFitness(),
                    "species not sorted: %d:%F > %d:%F",
                    i - 1,
                    species[i - 1]->getAverageAdjustedFitness(),
                    i,
                    species[i]->getAverageAdjustedFitness());
    }

    return args;
}

TEST(offspringTest) {

    /* speedup mutation for testing */
    Opts::getInstance()._addNeuronMutationPropability = 0.3;
    Opts::getInstance()._addConnectionMutationPropability = 0.5;

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};
    
    vector<shared_ptr<Species>> species;

    vector<shared_ptr<Organism>> organisms;
    organisms.push_back(make_shared<Organism>(2, 1));
    organisms.back()->calculateFittness(trainingInputs, trainingOutputs, 10, 15, 2);

    for (unsigned i = 0; i < 100; i++) {
        species.push_back(make_shared<Species>());
        unsigned nOrganisms = rand_range64(args, 1, 15);
        for (unsigned n = 0; n < nOrganisms; n++) {
            organisms.push_back(make_shared<Organism>(organisms.back()->clone()));
            organisms.back()->mutate();
            species.back()->add(organisms.back());
        }
        species.back()->calculateFittness(trainingInputs, trainingOutputs);
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


    std::sort(species.begin(), species.end(), SpeciesComparator());

    for (unsigned i = 1; i < species.size(); i++) {
        ASSERT_TRUE(species[i - 1]->getAverageAdjustedFitness() >=
                    species[i]->getAverageAdjustedFitness(),
                    "species not sorted: %d:%F > %d:%F",
                    i - 1,
                    species[i - 1]->getAverageAdjustedFitness(),
                    i,
                    species[i]->getAverageAdjustedFitness());
    }

    for (auto &s : species)
        ASSERT_EQUAL(s->createOffsprings(10).size(), 10);

    return args;
}

TEST(offspringChoosingTest) {

    /* speedup mutation for testing */
    Species species;

    species.add(make_shared<Organism>(2, 1));
    species.add(make_shared<Organism>(2, 1));
    species.add(make_shared<Organism>(2, 1));
    
    species.getOrganisms()[0]->setAdjustedFitness(0.1);
    species.getOrganisms()[1]->setAdjustedFitness(0.6);
    species.getOrganisms()[2]->setAdjustedFitness(0.3);

    double choosen[3] = { 0, 0, 0 };
    vector<unsigned> speciesChooses = species.chooseOrganisms(10000);

    for (const auto &c : speciesChooses)
        choosen[c] += 1;

    choosen[0] /= speciesChooses.size();
    choosen[2] /= speciesChooses.size();
    choosen[1] /= speciesChooses.size();

    ASSERT_TRUE(absDiff(choosen[0], 0.1) < 0.01);
    ASSERT_TRUE(absDiff(choosen[1], 0.6) < 0.01);
    ASSERT_TRUE(absDiff(choosen[2], 0.3) < 0.01);

    TEST_MSGD("0.1 ~= %F", choosen[0]);
    TEST_MSGD("0.6 ~= %F", choosen[1]);
    TEST_MSGD("0.3 ~= %F", choosen[2]);


    return args;
}
