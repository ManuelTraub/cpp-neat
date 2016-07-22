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

        sort(organisms.begin(), organisms.end(), orderOrganisms);

        for (unsigned i = 0; i < Opts::populationSize(); i++) {
            organisms[i] = make_shared<Organism>(organisms[i]->clone());
            organisms[i]->mutate();
        }
    }

    for (auto &o :organisms)
        o->calculateFittness(trainingInputs, trainingOutputs, 10, 15, 3);

    sort(organisms.begin(), organisms.end(), orderOrganisms);
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

        sort(organisms.begin(), organisms.end(), orderOrganisms);
        sort(species.begin(), species.end(), orderSpecies);

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

    sort(organisms.begin(), organisms.end(), orderOrganisms);
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

        for (auto &s : species)
            s->removeDeaths();
        
        for (auto &o :organisms)
            species[rand64(args) % 15]->add(o);

        for (auto &s : species)
            s->calculateFittness(trainingInputs, trainingOutputs);

        sort(organisms.begin(), organisms.end(), orderOrganisms);
        sort(species.begin(), species.end(), orderSpecies);

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

    sort(organisms.begin(), organisms.end(), orderOrganisms);
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
    SKIP_TEST("esel");

    Opts::getInstance()._speciesSimilarityThreshold = 0.5;
    Opts::getInstance()._structuralMutationBoost = 1;
    Opts::getInstance()._perturbeWeightRangeStart = -3;
    Opts::getInstance()._perturbeWeightRangeEnd = 3;
    Opts::getInstance()._addNeuronMutationPropability = 0.003;
    Opts::getInstance()._addConnectionMutationPropability = 0.005;

    /* training data */
    vector<vector<double>> trainingInputs;
    vector<vector<double>> trainingOutputs;

    for (unsigned i = 0; i < 100; i++) {
        unsigned randValue = rand64(args) % 4;
        if (randValue == 0) {
            trainingInputs.push_back({0, 0});
            trainingOutputs.push_back({0});
        } else if (randValue == 1) {
            trainingInputs.push_back({0, 1});
            trainingOutputs.push_back({1});
        } else if (randValue == 2) {
            trainingInputs.push_back({1, 0});
            trainingOutputs.push_back({1});
        } else if (randValue == 3) {
            trainingInputs.push_back({1, 1});
            trainingOutputs.push_back({0});
        }
    }


    Population population(trainingInputs, trainingOutputs);
    population.createInitialConnections();

    for (unsigned i = 0; i < 1500; i++) {
        uint64_t time = gettime_usec();
        TEST_MSGD("Generation [%d/1500]", i);

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
        printf("Generation [%d/1500]: ", i);
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

        /* check if we have a wining candidate */
        shared_ptr<Organism> best = population.getBestOrganism();
        Phenotype phenotype(best->getGenome());
        if (phenotype.run({0,0})[0] < 0.5 &&
            phenotype.run({0,1})[0] > 0.5 &&
            phenotype.run({1,0})[0] > 0.5 &&
            phenotype.run({1,1})[0] < 0.5)
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

TEST_N(speciesOffspringDistributionTest, 10) {
    SKIP_TEST("esel");

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {1}, {1}, {0}};

    Population population(trainingInputs, trainingOutputs);
    vector<shared_ptr<Species>> &species = population.getSpecies();

    species.push_back(make_shared<Species>());
    species.push_back(make_shared<Species>());
    
    species[0]->setAverageAdjustedFitness(0.1);
    species[1]->setAverageAdjustedFitness(0.6);
    species[2]->setAverageAdjustedFitness(0.3);

    double choosen[3] = { 0, 0, 0 };
    vector<unsigned> offspringsPerSpecies = population.calculateOffspringsPerSpecies(1000000);

    ASSERT_EQUAL(offspringsPerSpecies.size(), 3, "offsprings for more than 3 species");
    ASSERT_EQUAL(offspringsPerSpecies[0] + offspringsPerSpecies[1] + offspringsPerSpecies[2],
                 1000000, "more than 100000 offsprings");


    choosen[0] = ((double) offspringsPerSpecies[0]) / 1000000.0; 
    choosen[1] = ((double) offspringsPerSpecies[1]) / 1000000.0; 
    choosen[2] = ((double) offspringsPerSpecies[2]) / 1000000.0; 

    TEST_MSGD("0.1 ~= %F", choosen[0]);
    TEST_MSGD("0.6 ~= %F", choosen[1]);
    TEST_MSGD("0.3 ~= %F", choosen[2]);
    
    ASSERT_TRUE(absDiff(choosen[0], 0.1) < 0.01);
    ASSERT_TRUE(absDiff(choosen[1], 0.6) < 0.01);
    ASSERT_TRUE(absDiff(choosen[2], 0.3) < 0.01);

    return args;
}

double avgVector(vector<unsigned> v) {
    double avg = 0;
    for (const auto &e : v)
        avg += e;

    return avg / v.size();
}

TEST(binaryXORSpeedTest) {

    //Opts::getInstance()._speciesSimilarityThreshold = 0.5;
    //Opts::getInstance()._structuralMutationBoost = 1;
    //Opts::getInstance()._perturbeWeightRangeStart = -3;
    //Opts::getInstance()._perturbeWeightRangeEnd = 3;
    //Opts::getInstance()._addNeuronMutationPropability = 0.003;
    //Opts::getInstance()._addConnectionMutationPropability = 0.005;
    //Opts::getInstance()._mutationOnlyPropability = 1;

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {1}, {1}, {0}};

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 10000;
    const unsigned nGenerations = 100;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            Phenotype phenotype(best->getGenome());

            if (phenotype.run({0,0})[0] < 0.5 &&
                phenotype.run({0,1})[0] > 0.5 &&
                phenotype.run({1,0})[0] > 0.5 &&
                phenotype.run({1,1})[0] < 0.5) {
                break;
            }
        }

        shared_ptr<Organism> best = population.getBestOrganism();
        Phenotype phenotype(best->getGenome());

        if (i == nGenerations)
            printf("failed\n");
        else {

            unsigned enabledGenes = 0;
            for (const auto &c : best->getGenome().getConnections())
                if (c.second.isEnabled())
                    enabledGenes++;

            generations.push_back(i);
            nodes.push_back(best->getGenome().getNeurons().size());
            connections.push_back(enabledGenes);
            species.push_back(population.getSpecies().size());

            shared_ptr<Organism> best = population.getBestOrganism();
            Phenotype phenotype(best->getGenome());
            phenotype.saveAsSVG("bestOrganism" + itoa(connections.back()) + "c" + 
                                itoa(nodes.back()) + "n" + itoa(generations.back()) + "g.svg");

            printf("[%u] Generation %3u / %3.1F, Nodes %3u / %3.1F, Connections: %3u / %3.1F, Species %3u / %3.1F\n", r,
                   generations.back(), avgVector(generations),
                   nodes.back(), avgVector(nodes),
                   connections.back(), avgVector(connections),
                   species.back(), avgVector(species));
        }

    }

    return args;
}
