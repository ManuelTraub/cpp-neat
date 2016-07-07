#include "../src/utils.h"
#include "../src/Species.h"
#include "../src/Population.h"
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

/* returns the current milliseconds time */
uint64_t gettime_usec() {

  struct timeval time;
  if (gettimeofday(&time, NULL) == -1)
    return ((uint64_t) -1);

    return ((uint64_t) time.tv_sec) * ((uint64_t) 1000000) + 
           ((uint64_t) time.tv_usec);
}

TEST(basicOrganismSpeedTest) {
    SKIP_TEST("esel");

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};

    vector<shared_ptr<Organism>> organisms;

    for (unsigned i = 0; i < Opts::populationSize(); i++)
        organisms.push_back(make_shared<Organism>(2, 1));

    for (unsigned i = 0; i < 100; i++) {
        
        for (auto &o :organisms)
            o->calculateFittness(trainingInputs, trainingOutputs, 10, 15, 3);

        sort(organisms.begin(), organisms.end(), OrganismComparator());

        for (unsigned i = 0; i < Opts::populationSize(); i++) {
            organisms[i] = make_shared<Organism>(organisms[i]->clone());
            organisms[i]->mutate();
        }
    }

    for (auto &o :organisms)
        o->calculateFittness(trainingInputs, trainingOutputs, 10, 15, 3);

    sort(organisms.begin(), organisms.end(), OrganismComparator());
    Phenotype phenotype(organisms[0]->getGenome());

    phenotype.saveAsSVG("bestOrganism.svg");

    return args;
}

TEST(basicSpeciesSpeedTest) {
    SKIP_TEST("esel");

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};

    vector<shared_ptr<Organism>> organisms;
    vector<shared_ptr<Species>> species;

    for (unsigned i = 0; i < Opts::populationSize(); i++)
        organisms.push_back(make_shared<Organism>(2, 1));

    for (unsigned i = 0; i < 15; i++)
        species.push_back(make_shared<Species>());

    for (unsigned i = 0; i < 100; i++) {

        for (auto &s : species)
            s->removeDeaths();
        
        for (auto &o :organisms)
            species[rand64(args) % 15]->add(o);

        for (auto &s : species)
            s->calculateFittness(trainingInputs, trainingOutputs);

        sort(organisms.begin(), organisms.end(), OrganismComparator());
        sort(species.begin(), species.end(), SpeciesComparator());

        for (unsigned i = 0; i < Opts::populationSize(); i++) {
            organisms[i]->kill();
            organisms[i] = make_shared<Organism>(organisms[i]->clone());
            organisms[i]->mutate();
        }
    }

    for (auto &s : species)
        s->removeDeaths();
    
    for (auto &o :organisms)
        species[rand64(args) % 15]->add(o);

    for (auto &s : species)
        s->calculateFittness(trainingInputs, trainingOutputs);

    sort(organisms.begin(), organisms.end(), OrganismComparator());
    Phenotype phenotype(organisms[0]->getGenome());

    phenotype.saveAsSVG("bestOrganism.svg");

    return args;
}

TEST(advancedSpeciesSpeedTest) {
    SKIP_TEST("esel");

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};

    vector<shared_ptr<Organism>> organisms;
    vector<shared_ptr<Species>> species;

    for (unsigned i = 0; i < Opts::populationSize(); i++)
        organisms.push_back(make_shared<Organism>(2, 1));

    for (unsigned i = 0; i < 15; i++)
        species.push_back(make_shared<Species>());

    for (unsigned i = 0; i < 100; i++) {

        for (auto &s : species) {
            s->removeDeaths();
            s->calculateOldesGenome();
        }
        
        for (auto &o :organisms)
            species[rand64(args) % 15]->add(o);

        for (auto &s : species)
            s->calculateFittness(trainingInputs, trainingOutputs);

        sort(organisms.begin(), organisms.end(), OrganismComparator());
        sort(species.begin(), species.end(), SpeciesComparator());

        vector<shared_ptr<Organism>> nexGeneration;

        for (unsigned i = 0; i < Opts::populationSize(); i++) {
            unsigned index = rand64(args) % 15;
            while (species[index]->size() == 0)
                index = rand64(args) % 15;

            nexGeneration.push_back(species[index]->createOffsprings(1)[0]);
        }

        for (unsigned i = 0; i < Opts::populationSize(); i++) {
            organisms[i]->kill();
            organisms[i] = nexGeneration[i];
        }
    }

    for (auto &s : species)
        s->removeDeaths();
    
    for (auto &o :organisms)
        species[rand64(args) % 15]->add(o);

    for (auto &s : species)
        s->calculateFittness(trainingInputs, trainingOutputs);

    sort(organisms.begin(), organisms.end(), OrganismComparator());
    Phenotype phenotype(organisms[0]->getGenome());

    phenotype.saveAsSVG("bestOrganism.svg");

    return args;
}

TEST(basicGenerationChangeSpeedTest) {
    SKIP_TEST("esel");

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};

    Population population(trainingInputs, trainingOutputs);

    for (unsigned i = 0; i < 500; i++) {
        uint64_t time = gettime_usec();
        TEST_MSGD("Generation [%d/500]", i);

        /* calculate the Organisms for reproduction */
        vector<shared_ptr<Organism>> lastGeneration = population.prepareGenerationChange();

        /* check that there are not to much survivors */
        ASSERT_TRUE(lastGeneration.size() < Opts::populationSize() * Opts::survivalRate() + 1,
                    "to much survivors: %lu < %F", 
                    lastGeneration.size(),
                    Opts::populationSize() * Opts::survivalRate() + 1);

        /* calculate the interspecies offsprings 
         * (add one offsprings more based on the propability given from the decimal part )
         */
        vector<shared_ptr<Organism>> interSpeciesOffsprings = population.createInterSpeciesOffsprings();
        
        /* check that there are not to much inter species offsprings */
        ASSERT_TRUE(interSpeciesOffsprings.size() <
                    Opts::populationSize() * Opts::interSpeciesMatingPropability() + 1,
                    "to much inter species offsprings: %lu < %F",
                    interSpeciesOffsprings.size(),
                    Opts::populationSize() * Opts::interSpeciesMatingPropability() + 1);

        /* calculate the number of offsprings per species */
        vector<unsigned> offspringsPerSpecies = population.calculateOffspringsPerSpecies(
            Opts::populationSize() - interSpeciesOffsprings.size()
        );

        /* check that the overall ofssprings are equal to the population size */
        uint64_t overallOffsprings = interSpeciesOffsprings.size();
        for (const auto &n : offspringsPerSpecies)
            overallOffsprings += n;

        ASSERT_EQUAL(overallOffsprings, Opts::populationSize(), 
                     "to much or to less offsprings: %lu != %u",
                     overallOffsprings, Opts::populationSize());

        /* calculate the inspecies offsprings */
        vector<shared_ptr<Organism>> inSpeciesOffsprings = population.createInSpeciesOffsprings(
            offspringsPerSpecies
        );
        
        /* check that there arent to much in species offsprings */
        ASSERT_TRUE(inSpeciesOffsprings.size() <= 
                    Opts::populationSize() - interSpeciesOffsprings.size(),
                    "to much in species offsprings: %lu <= %lu",
                    inSpeciesOffsprings.size(),
                    Opts::populationSize() - interSpeciesOffsprings.size());

        /* sort the offsprings into species */
        population.sortOffspringsIntoSpecies(interSpeciesOffsprings);
        population.sortOffspringsIntoSpecies(inSpeciesOffsprings);
        
        /* finish the generation change by doing cleanups */
        population.finishGenerationChange(lastGeneration);

        /* check the number of Organisms */
        vector<shared_ptr<Organism>> organisms;
        unsigned si = 0;
        for (const auto &s : population.getSpecies()) {
            vector<shared_ptr<Organism>> speciesOrganisms = s->getOrganisms();
            organisms.insert(organisms.end(), speciesOrganisms.begin(), speciesOrganisms.end());
            printf("%u: %u, ", si, s->size()); si++;
        }
        printf("\n");

        ASSERT_EQUAL(organisms.size(), Opts::populationSize(),
                     "to much or to less organisms: %lu != %u",
                     organisms.size(),
                     Opts::populationSize());

        if (gettime_usec() - time > 3000000)
            break;
    }

    shared_ptr<Organism> best = population.getBestOrganism();
    Phenotype phenotype(best->getGenome());

    phenotype.saveAsSVG("bestOrganism.svg");

    ASSERT_TRUE(phenotype.run({0,0})[0] < 0.5, "AND(0,0) = 0: %F", phenotype.run({0,0})[0]);
    ASSERT_TRUE(phenotype.run({0,1})[0] < 0.5, "AND(0,1) = 0: %F", phenotype.run({0,1})[0]);
    ASSERT_TRUE(phenotype.run({1,0})[0] < 0.5, "AND(1,0) = 0: %F", phenotype.run({1,0})[0]);
    ASSERT_TRUE(phenotype.run({1,1})[0] > 0.5, "AND(1,1) = 1: %F", phenotype.run({1,1})[0]);

    TEST_MSGD("AND(0,0) = 0: %F", phenotype.run({0,0})[0]);
    TEST_MSGD("AND(0,1) = 0: %F", phenotype.run({0,1})[0]);
    TEST_MSGD("AND(1,0) = 0: %F", phenotype.run({1,0})[0]);
    TEST_MSGD("AND(1,1) = 1: %F", phenotype.run({1,1})[0]);

    return args;
}

TEST(basicBinaryXORTest) {

    Opts::getInstance()._speciesSimilarityThreshold = 0.2;
    Opts::getInstance()._addNeuronMutationPropability = 0.003;
    Opts::getInstance()._addConnectionMutationPropability = 0.005;

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {1}, {1}, {0}};

    Population population(trainingInputs, trainingOutputs);

    for (unsigned i = 0; i < 1000; i++) {
        uint64_t time = gettime_usec();
        TEST_MSGD("Generation [%d/1000]", i);

        /* calculate the Organisms for reproduction */
        vector<shared_ptr<Organism>> lastGeneration = population.prepareGenerationChange();

        /* check that there are not to much survivors */
        ASSERT_TRUE(lastGeneration.size() < Opts::populationSize() * Opts::survivalRate() + 1,
                    "to much survivors: %lu < %F", 
                    lastGeneration.size(),
                    Opts::populationSize() * Opts::survivalRate() + 1);

        /* calculate the interspecies offsprings 
         * (add one offsprings more based on the propability given from the decimal part )
         */
        vector<shared_ptr<Organism>> interSpeciesOffsprings = population.createInterSpeciesOffsprings();
        
        /* check that there are not to much inter species offsprings */
        ASSERT_TRUE(interSpeciesOffsprings.size() <
                    Opts::populationSize() * Opts::interSpeciesMatingPropability() + 1,
                    "to much inter species offsprings: %lu < %F",
                    interSpeciesOffsprings.size(),
                    Opts::populationSize() * Opts::interSpeciesMatingPropability() + 1);

        /* calculate the number of offsprings per species */
        vector<unsigned> offspringsPerSpecies = population.calculateOffspringsPerSpecies(
            Opts::populationSize() - interSpeciesOffsprings.size()
        );

        /* check that the overall ofssprings are equal to the population size */
        uint64_t overallOffsprings = interSpeciesOffsprings.size();
        for (const auto &n : offspringsPerSpecies)
            overallOffsprings += n;

        ASSERT_EQUAL(overallOffsprings, Opts::populationSize(), 
                     "to much or to less offsprings: %lu != %u",
                     overallOffsprings, Opts::populationSize());

        /* calculate the inspecies offsprings */
        vector<shared_ptr<Organism>> inSpeciesOffsprings = population.createInSpeciesOffsprings(
            offspringsPerSpecies
        );
        
        /* check that there arent to much in species offsprings */
        ASSERT_TRUE(inSpeciesOffsprings.size() <= 
                    Opts::populationSize() - interSpeciesOffsprings.size(),
                    "to much in species offsprings: %lu <= %lu",
                    inSpeciesOffsprings.size(),
                    Opts::populationSize() - interSpeciesOffsprings.size());

        /* sort the offsprings into species */
        population.sortOffspringsIntoSpecies(interSpeciesOffsprings);
        population.sortOffspringsIntoSpecies(inSpeciesOffsprings);
        
        /* finish the generation change by doing cleanups */
        population.finishGenerationChange(lastGeneration);

        /* check the number of Organisms */
        vector<shared_ptr<Organism>> organisms;
        unsigned si = 0;
        for (const auto &s : population.getSpecies()) {
            vector<shared_ptr<Organism>> speciesOrganisms = s->getOrganisms();
            organisms.insert(organisms.end(), speciesOrganisms.begin(), speciesOrganisms.end());
            printf("%u: %u, ", si, s->size()); si++;
        }
        printf("\n");

        ASSERT_EQUAL(organisms.size(), Opts::populationSize(),
                     "to much or to less organisms: %lu != %u",
                     organisms.size(),
                     Opts::populationSize());

        if (gettime_usec() - time > 3000000)
            break;
    }

    shared_ptr<Organism> best = population.getBestOrganism();
    Phenotype phenotype(best->getGenome());

    phenotype.saveAsSVG("bestOrganism.svg");

    TEST_MSGI("XOR(0,0) = 0: %F", phenotype.run({0,0})[0]);
    TEST_MSGI("XOR(0,1) = 1: %F", phenotype.run({0,1})[0]);
    TEST_MSGI("XOR(1,0) = 1: %F", phenotype.run({1,0})[0]);
    TEST_MSGI("XOR(1,1) = 0: %F", phenotype.run({1,1})[0]);

    ASSERT_TRUE(phenotype.run({0,0})[0] < 0.5, "XOR(0,0) = 0: %F", phenotype.run({0,0})[0]);
    ASSERT_TRUE(phenotype.run({0,1})[0] > 0.5, "XOR(0,1) = 1: %F", phenotype.run({0,1})[0]);
    ASSERT_TRUE(phenotype.run({1,0})[0] > 0.5, "XOR(1,0) = 1: %F", phenotype.run({1,0})[0]);
    ASSERT_TRUE(phenotype.run({1,1})[0] < 0.5, "XOR(1,1) = 0: %F", phenotype.run({1,1})[0]);

    return args;
}
