#include "../../src/AI/utils.h"
#include "../../src/AI/Species.h"
#include "../../src/AI/Population.h"
#include "../../src/AI/Opts.h"

#include <vector>
#include <memory>
#include <algorithm>

using std::shared_ptr;
using std::make_shared;
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


TEST(basicOrganismSpeedTest) {

    /* training data */
    Opts opts;
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};

    vector<shared_ptr<Organism>> organisms;

    for (unsigned i = 0; i < opts.populationSize(); i++)
        organisms.push_back(make_shared<Organism>(2, 1, opts));

    for (unsigned i = 0; i < 100; i++) {
        
        for (auto &o :organisms)
            o->calculateFittness(trainingInputs, trainingOutputs);

        sort(organisms.begin(), organisms.end(), orderOrganisms);

        for (unsigned i = 0; i < opts.populationSize(); i++) {
            organisms[i] = make_shared<Organism>(organisms[i]->clone());
            organisms[i]->mutate();
        }
    }

    for (auto &o :organisms)
        o->calculateFittness(trainingInputs, trainingOutputs);

    sort(organisms.begin(), organisms.end(), orderOrganisms);
    Phenotype phenotype(organisms[0]->getGenome(), opts);

    phenotype.saveAsSVG("basicOrganismSpeedTest.svg");

    return args;
}

TEST(basicSpeciesSpeedTest) {

    /* training data */
    Opts opts;
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};

    vector<shared_ptr<Organism>> organisms;
    vector<shared_ptr<Species>> species;

    for (unsigned i = 0; i < opts.populationSize(); i++)
        organisms.push_back(make_shared<Organism>(2, 1, opts));

    for (unsigned i = 0; i < 15; i++)
        species.push_back(make_shared<Species>(opts));

    for (unsigned i = 0; i < 100; i++) {

        for (auto &s : species)
            s->removeDeaths();
        
        for (auto &o :organisms)
            species[rand64(args) % 15]->add(o);

        for (auto &s : species)
            s->calculateFittness(trainingInputs, trainingOutputs);

        sort(organisms.begin(), organisms.end(), orderOrganisms);
        sort(species.begin(), species.end(), orderSpecies);

        for (unsigned i = 0; i < opts.populationSize(); i++) {
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
    Phenotype phenotype(organisms[0]->getGenome(), opts);

    phenotype.saveAsSVG("basicSpeciesSpeedTest.svg");

    return args;
}

TEST(advancedSpeciesSpeedTest) {

    /* training data */
    Opts opts;
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};

    vector<shared_ptr<Organism>> organisms;
    vector<shared_ptr<Species>> species;

    for (unsigned i = 0; i < opts.populationSize(); i++)
        organisms.push_back(make_shared<Organism>(2, 1, opts));

    for (unsigned i = 0; i < 15; i++)
        species.push_back(make_shared<Species>(opts));

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

        for (unsigned i = 0; i < opts.populationSize(); i++) {
            unsigned index = rand64(args) % 15;
            while (species[index]->size() == 0)
                index = rand64(args) % 15;

            nexGeneration.push_back(species[index]->createOffsprings(1)[0]);
        }

        for (unsigned i = 0; i < opts.populationSize(); i++) {
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
    Phenotype phenotype(organisms[0]->getGenome(), opts);

    phenotype.saveAsSVG("advancedSpeciesSpeedTest.svg");

    return args;
}

TEST(basicGenerationChangeSpeedTest) {

    /* training data */
    Opts opts;
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};

    Population population(trainingInputs, trainingOutputs, opts);

    for (unsigned i = 0; i < 500; i++) {
        uint64_t time = gettime_usec();
        TEST_MSGD("Generation [%d/500]", i);

        /* calculate the Organisms for reproduction */
        vector<shared_ptr<Organism>> lastGeneration = population.prepareGenerationChange();

        /* calculate the interspecies offsprings 
         * (add one offsprings more based on the propability given from the decimal part )
         */
        vector<shared_ptr<Organism>> interSpeciesOffsprings = population.createInterSpeciesOffsprings();
        
        /* check that there are not to much inter species offsprings */
        ASSERT_TRUE(interSpeciesOffsprings.size() <
                    opts.populationSize() * opts.interSpeciesMatingPropability() + 1,
                    "to much inter species offsprings: %lu < %F",
                    interSpeciesOffsprings.size(),
                    opts.populationSize() * opts.interSpeciesMatingPropability() + 1);

        /* calculate the number of offsprings per species */
        vector<unsigned> offspringsPerSpecies = population.calculateOffspringsPerSpecies(
            opts.populationSize() - interSpeciesOffsprings.size()
        );

        /* check that the overall ofssprings are equal to the population size */
        uint64_t overallOffsprings = interSpeciesOffsprings.size();
        for (const auto &n : offspringsPerSpecies)
            overallOffsprings += n;

        ASSERT_EQUAL(overallOffsprings, opts.populationSize(), 
                     "to much or to less offsprings: %lu != %u",
                     overallOffsprings, opts.populationSize());

        /* calculate the inspecies offsprings */
        vector<shared_ptr<Organism>> inSpeciesOffsprings = population.createInSpeciesOffsprings(
            offspringsPerSpecies
        );
        
        /* check that there arent to much in species offsprings */
        ASSERT_TRUE(inSpeciesOffsprings.size() <= 
                    opts.populationSize() - interSpeciesOffsprings.size(),
                    "to much in species offsprings: %lu <= %lu",
                    inSpeciesOffsprings.size(),
                    opts.populationSize() - interSpeciesOffsprings.size());

        /* sort the offsprings into species */
        population.sortOffspringsIntoSpecies(interSpeciesOffsprings);
        population.sortOffspringsIntoSpecies(inSpeciesOffsprings);
        
        /* finish the generation change by doing cleanups */
        population.finishGenerationChange(lastGeneration);

        /* check the number of Organisms */
        vector<shared_ptr<Organism>> organisms;
        for (const auto &s : population.getSpecies()) {
            vector<shared_ptr<Organism>> speciesOrganisms = s->getOrganisms();
            organisms.insert(organisms.end(), speciesOrganisms.begin(), speciesOrganisms.end());
        }

        ASSERT_EQUAL(organisms.size(), opts.populationSize(),
                     "to much or to less organisms: %lu != %u",
                     organisms.size(),
                     opts.populationSize());

        if (gettime_usec() - time > 3000000)
            ASSERT_FAIL("to much time needed");

        /* check if we have a wining candidate */
        shared_ptr<Organism> best = population.getBestOrganism();
        Phenotype phenotype(best->getGenome(), opts);
        if (phenotype.maxDerivation(trainingInputs, trainingOutputs) < 0.5)
            break;
    }

    shared_ptr<Organism> best = population.getBestOrganism();
    Phenotype phenotype(best->getGenome(), opts);

    phenotype.saveAsSVG("basicGenerationChangeSpeedTest.svg");

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

    /* training data */
    Opts opts;
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {1}, {1}, {0}};

    Population population(trainingInputs, trainingOutputs, opts);
    population.createInitialConnections();

    for (unsigned i = 0; i < 1500; i++) {
        uint64_t time = gettime_usec();
        TEST_MSGD("Generation [%d/1500]", i);

        /* calculate the Organisms for reproduction */
        vector<shared_ptr<Organism>> lastGeneration = population.prepareGenerationChange();

        /* calculate the interspecies offsprings 
         * (add one offsprings more based on the propability given from the decimal part )
         */
        vector<shared_ptr<Organism>> interSpeciesOffsprings = population.createInterSpeciesOffsprings();
        
        /* check that there are not to much inter species offsprings */
        ASSERT_TRUE(interSpeciesOffsprings.size() <
                    opts.populationSize() * opts.interSpeciesMatingPropability() + 1,
                    "to much inter species offsprings: %lu < %F",
                    interSpeciesOffsprings.size(),
                    opts.populationSize() * opts.interSpeciesMatingPropability() + 1);

        /* calculate the number of offsprings per species */
        vector<unsigned> offspringsPerSpecies = population.calculateOffspringsPerSpecies(
            opts.populationSize() - interSpeciesOffsprings.size()
        );

        /* check that the overall ofssprings are equal to the population size */
        uint64_t overallOffsprings = interSpeciesOffsprings.size();
        for (const auto &n : offspringsPerSpecies)
            overallOffsprings += n;

        ASSERT_EQUAL(overallOffsprings, opts.populationSize(), 
                     "to much or to less offsprings: %lu != %u",
                     overallOffsprings, opts.populationSize());

        /* calculate the inspecies offsprings */
        vector<shared_ptr<Organism>> inSpeciesOffsprings = population.createInSpeciesOffsprings(
            offspringsPerSpecies
        );
        
        /* check that there arent to much in species offsprings */
        ASSERT_TRUE(inSpeciesOffsprings.size() <= 
                    opts.populationSize() - interSpeciesOffsprings.size(),
                    "to much in species offsprings: %lu <= %lu",
                    inSpeciesOffsprings.size(),
                    opts.populationSize() - interSpeciesOffsprings.size());

        /* sort the offsprings into species */
        population.sortOffspringsIntoSpecies(interSpeciesOffsprings);
        population.sortOffspringsIntoSpecies(inSpeciesOffsprings);
        
        /* finish the generation change by doing cleanups */
        population.finishGenerationChange(lastGeneration);

        /* check the number of Organisms */
        vector<shared_ptr<Organism>> organisms;
        for (const auto &s : population.getSpecies()) {
            vector<shared_ptr<Organism>> speciesOrganisms = s->getOrganisms();
            organisms.insert(organisms.end(), speciesOrganisms.begin(), speciesOrganisms.end());
        }

        ASSERT_EQUAL(organisms.size(), opts.populationSize(),
                     "to much or to less organisms: %lu != %u",
                     organisms.size(),
                     opts.populationSize());

        if (gettime_usec() - time > 3000000)
            ASSERT_FAIL("to much time needed");

        /* check if we have a wining candidate */
        shared_ptr<Organism> best = population.getBestOrganism();
        Phenotype phenotype(best->getGenome(), opts);
        if (phenotype.maxDerivation(trainingInputs, trainingOutputs) < 0.5)
            break;
    }

    shared_ptr<Organism> best = population.getBestOrganism();
    Phenotype phenotype(best->getGenome(), opts);

    phenotype.saveAsSVG("basicBinaryXORTest.svg");

    TEST_MSGD("XOR(0,0) = 0: %F", phenotype.run({0,0})[0]);
    TEST_MSGD("XOR(0,1) = 1: %F", phenotype.run({0,1})[0]);
    TEST_MSGD("XOR(1,0) = 1: %F", phenotype.run({1,0})[0]);
    TEST_MSGD("XOR(1,1) = 0: %F", phenotype.run({1,1})[0]);

    ASSERT_TRUE(phenotype.run({0,0})[0] < 0.5, "XOR(0,0) = 0: %F", phenotype.run({0,0})[0]);
    ASSERT_TRUE(phenotype.run({0,1})[0] > 0.5, "XOR(0,1) = 1: %F", phenotype.run({0,1})[0]);
    ASSERT_TRUE(phenotype.run({1,0})[0] > 0.5, "XOR(1,0) = 1: %F", phenotype.run({1,0})[0]);
    ASSERT_TRUE(phenotype.run({1,1})[0] < 0.5, "XOR(1,1) = 0: %F", phenotype.run({1,1})[0]);

    return args;
}

double avgVector(vector<unsigned> v) {
    double avg = 0;
    for (const auto &e : v)
        avg += e;

    return avg / v.size();
}

TEST(binaryXORQualityTest) {

    // training data
    Opts opts;
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {1}, {1}, {0}};
    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 300;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            Phenotype phenotype(best->getGenome(), opts);

            if (phenotype.maxDerivation(trainingInputs, trainingOutputs) < 0.5)
                break;
        }

        shared_ptr<Organism> best = population.getBestOrganism();
        Phenotype phenotype(best->getGenome(), opts);

        if (i == nGenerations) {
            printf("failed\n");
            phenotype.saveAsSVG("failedOrganism" + itoa(r) + ".svg");
        } else {

            unsigned enabledGenes = 0;
            for (const auto &c : best->getGenome().getConnections())
                if (c.second.isEnabled())
                    enabledGenes++;

            generations.push_back(i);
            nodes.push_back(best->getGenome().getNeurons().size());
            connections.push_back(enabledGenes);
            species.push_back(population.getSpecies().size());

            phenotype.saveAsSVG("bestOrganism" + itoa(connections.back()) + "c" + 
                                itoa(nodes.back()) + "n" + itoa(generations.back()) + "g.svg");

            TEST_MSGD("[%u] Generation %3u / %3.1F, Nodes %3u / %3.1F, Connections: %3u / %3.1F, Species %3u / %3.1F", r,
                      generations.back(), avgVector(generations),
                      nodes.back(), avgVector(nodes),
                      connections.back(), avgVector(connections),
                      species.back(), avgVector(species));

            if (r > nRuns / 1.5) {
                ASSERT_TRUE(avgVector(generations) < 32);
                ASSERT_TRUE(avgVector(nodes) < 7);
                ASSERT_TRUE(avgVector(connections) < 11);
                ASSERT_TRUE(avgVector(species) < 18);
            }
        }

    }

    return args;
}

TEST(binaryXORZeroStartConectionsQualityTest) {
    
    Opts opts;
    opts.mutateGradientPropability(0.0);
    opts.mutationMaxPertubeWeightRangeStart(-8);
    opts.mutationMaxPertubeWeightRangeEnd(8);

    // training data
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {1}, {1}, {0}};

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 300;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts);

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            Phenotype phenotype(best->getGenome(), opts);

            if (phenotype.maxDerivation(trainingInputs, trainingOutputs) < 0.5)
                break;
        }

        shared_ptr<Organism> best = population.getBestOrganism();
        Phenotype phenotype(best->getGenome(), opts);

        if (i == nGenerations) {
            printf("failed\n");
            phenotype.saveAsSVG("failedOrganism" + itoa(r) + ".svg");
        } else {

            unsigned enabledGenes = 0;
            for (const auto &c : best->getGenome().getConnections())
                if (c.second.isEnabled())
                    enabledGenes++;

            generations.push_back(i);
            nodes.push_back(best->getGenome().getNeurons().size());
            connections.push_back(enabledGenes);
            species.push_back(population.getSpecies().size());

            phenotype.saveAsSVG("bestOrganism" + itoa(connections.back()) + "c" + 
                                itoa(nodes.back()) + "n" + itoa(generations.back()) + "g.svg");

            TEST_MSGD("[%u] Generation %3u / %3.1F, Nodes %3u / %3.1F, Connections: %3u / %3.1F, Species %3u / %3.1F", r,
                      generations.back(), avgVector(generations),
                      nodes.back(), avgVector(nodes),
                      connections.back(), avgVector(connections),
                      species.back(), avgVector(species));

            if (r > nRuns / 1.5) {
                ASSERT_TRUE(avgVector(generations) < 35);
                ASSERT_TRUE(avgVector(nodes) < 7);
                ASSERT_TRUE(avgVector(connections) < 11);
                ASSERT_TRUE(avgVector(species) < 18);
            }
        }

    }

    return args;
}

TEST(basicRecurencyTest) {
    
    Opts opts;
    opts.netActivationFunction(Opts::ActivationFunction::RELU);
    opts.weightMultiplier(1.0);
    opts.speciesSimilarityThreshold(5.0);
    opts.allowRecurentConnections(true);
    opts.shuffleTrainingData(false);

    // training data
    vector<vector<double>> trainingInputs  = {{1}, {1}, {1}, {1}, {1}, {1}, {1}, {1}, {1}, {1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {1}};

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 500;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            Phenotype phenotype(best->getGenome(), opts);

            if (phenotype.maxDerivation(trainingInputs, trainingOutputs) < 0.5)
                break;
        }

        shared_ptr<Organism> best = population.getBestOrganism();
        Phenotype phenotype(best->getGenome(), opts);

        if (i == nGenerations) {
            printf("failed\n");
            phenotype.saveAsSVG("failedOrganism" + itoa(r) + ".svg");
        } else {

            unsigned enabledGenes = 0;
            for (const auto &c : best->getGenome().getConnections())
                if (c.second.isEnabled())
                    enabledGenes++;

            generations.push_back(i);
            nodes.push_back(best->getGenome().getNeurons().size());
            connections.push_back(enabledGenes);
            species.push_back(population.getSpecies().size());

            phenotype.saveAsSVG("bestOrganism" + itoa(connections.back()) + "c" + 
                                itoa(nodes.back()) + "n" + itoa(generations.back()) + "g.svg");

            TEST_MSGD("[%u] Generation %3u / %3.1F, Nodes %3u / %3.1F, Connections: %3u / %3.1F, Species %3u / %3.1F", r,
                      generations.back(), avgVector(generations),
                      nodes.back(), avgVector(nodes),
                      connections.back(), avgVector(connections),
                      species.back(), avgVector(species));

            /* * /
            Phenotype phenotype2(best->getGenome(), opts);
            Phenotype phenotype3(best->getGenome(), opts);
            for (unsigned t = 0; t < trainingOutputs.size(); t++)
                printf("%f  =>  %f\n", trainingOutputs[t][0], phenotype2.run(trainingInputs[t])[0]);
            
            printf("maxDerivation: %f\n", phenotype3.maxDerivation(trainingInputs, trainingOutputs));
            / * */

            if (r > nRuns / 1.5) {
                ASSERT_TRUE(avgVector(generations) < 90);
                ASSERT_TRUE(avgVector(nodes) < 9);
                ASSERT_TRUE(avgVector(connections) < 19);
                ASSERT_TRUE(avgVector(species) < 25);
            }
        }

    }

    return args;
}

TEST(binaryXORQualityTanhTest) {

    Opts opts;
    opts.netActivationFunction(Opts::ActivationFunction::TANH);

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {1}, {1}, {0}};
    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 300;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            Phenotype phenotype(best->getGenome(), opts);

            if (phenotype.maxDerivation(trainingInputs, trainingOutputs) < 0.5)
                break;
        }

        shared_ptr<Organism> best = population.getBestOrganism();
        Phenotype phenotype(best->getGenome(), opts);

        if (i == nGenerations) {
            printf("failed\n");
            phenotype.saveAsSVG("failedOrganism" + itoa(r) + ".svg");
        } else {

            unsigned enabledGenes = 0;
            for (const auto &c : best->getGenome().getConnections())
                if (c.second.isEnabled())
                    enabledGenes++;

            generations.push_back(i);
            nodes.push_back(best->getGenome().getNeurons().size());
            connections.push_back(enabledGenes);
            species.push_back(population.getSpecies().size());

            phenotype.saveAsSVG("bestOrganism" + itoa(connections.back()) + "c" + 
                                itoa(nodes.back()) + "n" + itoa(generations.back()) + "g.svg");

            TEST_MSGD("[%u] Generation %3u / %3.1F, Nodes %3u / %3.1F, Connections: %3u / %3.1F, Species %3u / %3.1F", r,
                      generations.back(), avgVector(generations),
                      nodes.back(), avgVector(nodes),
                      connections.back(), avgVector(connections),
                      species.back(), avgVector(species));

            if (r > nRuns / 1.5) {
                ASSERT_TRUE(avgVector(generations) < 19);
                ASSERT_TRUE(avgVector(nodes) < 7);
                ASSERT_TRUE(avgVector(connections) < 11);
                ASSERT_TRUE(avgVector(species) < 18);
            }
        }

    }

    return args;
}

TEST(binaryXORZeroStartConectionsQualityTanhTest) {

    Opts opts;
    opts.netActivationFunction(Opts::ActivationFunction::TANH);

    /* training data */
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {1}, {1}, {0}};

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 300;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts);

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            Phenotype phenotype(best->getGenome(), opts);

            if (phenotype.maxDerivation(trainingInputs, trainingOutputs) < 0.5)
                break;
        }

        shared_ptr<Organism> best = population.getBestOrganism();
        Phenotype phenotype(best->getGenome(), opts);

        if (i == nGenerations) {
            printf("failed\n");
            phenotype.saveAsSVG("failedOrganism" + itoa(r) + ".svg");
        } else {

            unsigned enabledGenes = 0;
            for (const auto &c : best->getGenome().getConnections())
                if (c.second.isEnabled())
                    enabledGenes++;

            generations.push_back(i);
            nodes.push_back(best->getGenome().getNeurons().size());
            connections.push_back(enabledGenes);
            species.push_back(population.getSpecies().size());

            phenotype.saveAsSVG("bestOrganism" + itoa(connections.back()) + "c" + 
                                itoa(nodes.back()) + "n" + itoa(generations.back()) + "g.svg");

            TEST_MSGD("[%u] Generation %3u / %3.1F, Nodes %3u / %3.1F, Connections: %3u / %3.1F, Species %3u / %3.1F", r,
                      generations.back(), avgVector(generations),
                      nodes.back(), avgVector(nodes),
                      connections.back(), avgVector(connections),
                      species.back(), avgVector(species));

            if (r > nRuns / 1.5) {
                ASSERT_TRUE(avgVector(generations) < 25);
                ASSERT_TRUE(avgVector(nodes) < 7);
                ASSERT_TRUE(avgVector(connections) < 10);
                ASSERT_TRUE(avgVector(species) < 19);
            }
        }

    }

    return args;
}
