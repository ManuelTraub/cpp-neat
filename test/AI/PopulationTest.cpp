#include "../../src/utils/utils.h"
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

double avgVector(vector<unsigned> v) {
    double avg = 0;
    for (const auto &e : v)
        avg += e;

    return avg / v.size();
}

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

    // training data
    Opts opts;
    GeneHistory geneHistory(opts);
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};

    vector<shared_ptr<Organism>> organisms;

    for (unsigned i = 0; i < opts.populationSize(); i++)
        organisms.push_back(make_shared<Organism>(2, 1, opts, geneHistory));

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

    // training data
    Opts opts;
    GeneHistory geneHistory(opts);
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};

    vector<shared_ptr<Organism>> organisms;
    vector<shared_ptr<Species>> species;

    for (unsigned i = 0; i < opts.populationSize(); i++)
        organisms.push_back(make_shared<Organism>(2, 1, opts, geneHistory));

    for (unsigned i = 0; i < 15; i++)
        species.push_back(make_shared<Species>(opts, geneHistory));

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

    // training data
    Opts opts;
    GeneHistory geneHistory(opts);
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};

    vector<shared_ptr<Organism>> organisms;
    vector<shared_ptr<Species>> species;

    for (unsigned i = 0; i < opts.populationSize(); i++)
        organisms.push_back(make_shared<Organism>(2, 1, opts, geneHistory));

    for (unsigned i = 0; i < 15; i++)
        species.push_back(make_shared<Species>(opts, geneHistory));

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

    // training data
    Opts opts;
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {1}};

    Population population(trainingInputs, trainingOutputs, opts);

    for (unsigned i = 0; i < 500; i++) {
        uint64_t time = gettime_usec();
        TEST_MSGD("Generation [%d/500]", i);

        // calculate the Organisms for reproduction
        vector<shared_ptr<Organism>> lastGeneration = population.prepareGenerationChange();

        // calculate the interspecies offsprings 
        // (add one offsprings more based on the propability given from the decimal part )
        //
        vector<shared_ptr<Organism>> interSpeciesOffsprings = population.createInterSpeciesOffsprings();
        
        // check that there are not to much inter species offsprings
        ASSERT_TRUE(interSpeciesOffsprings.size() <
                    opts.populationSize() * opts.interSpeciesMatingPropability() + 1,
                    "to much inter species offsprings: %lu < %F",
                    interSpeciesOffsprings.size(),
                    opts.populationSize() * opts.interSpeciesMatingPropability() + 1);

        // calculate the number of offsprings per species
        vector<unsigned> offspringsPerSpecies = population.calculateOffspringsPerSpecies(
            opts.populationSize() - interSpeciesOffsprings.size()
        );

        // check that the overall ofssprings are equal to the population size
        uint64_t overallOffsprings = interSpeciesOffsprings.size();
        for (const auto &n : offspringsPerSpecies)
            overallOffsprings += n;

        ASSERT_EQUAL(overallOffsprings, opts.populationSize(), 
                     "to much or to less offsprings: %lu != %u",
                     overallOffsprings, opts.populationSize());

        // calculate the inspecies offsprings
        vector<shared_ptr<Organism>> inSpeciesOffsprings = population.createInSpeciesOffsprings(
            offspringsPerSpecies
        );
        
        // check that there arent to much in species offsprings
        ASSERT_TRUE(inSpeciesOffsprings.size() <= 
                    opts.populationSize() - interSpeciesOffsprings.size(),
                    "to much in species offsprings: %lu <= %lu",
                    inSpeciesOffsprings.size(),
                    opts.populationSize() - interSpeciesOffsprings.size());

        // sort the offsprings into species
        population.sortOffspringsIntoSpecies(interSpeciesOffsprings);
        population.sortOffspringsIntoSpecies(inSpeciesOffsprings);
        
        // finish the generation change by doing cleanups
        population.finishGenerationChange(lastGeneration);

        // check the number of Organisms
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

        // check if we have a wining candidate
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

    // training data
    Opts opts;
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {1}, {1}, {0}};

    Population population(trainingInputs, trainingOutputs, opts);
    population.createInitialConnections();

    for (unsigned i = 0; i < 1500; i++) {
        uint64_t time = gettime_usec();
        TEST_MSGD("Generation [%d/1500]", i);

        // calculate the Organisms for reproduction
        vector<shared_ptr<Organism>> lastGeneration = population.prepareGenerationChange();

        // calculate the interspecies offsprings 
        // (add one offsprings more based on the propability given from the decimal part )
        //
        vector<shared_ptr<Organism>> interSpeciesOffsprings = population.createInterSpeciesOffsprings();
        
        // check that there are not to much inter species offsprings 
        ASSERT_TRUE(interSpeciesOffsprings.size() <
                    opts.populationSize() * opts.interSpeciesMatingPropability() + 1,
                    "to much inter species offsprings: %lu < %F",
                    interSpeciesOffsprings.size(),
                    opts.populationSize() * opts.interSpeciesMatingPropability() + 1);

        // calculate the number of offsprings per species 
        vector<unsigned> offspringsPerSpecies = population.calculateOffspringsPerSpecies(
            opts.populationSize() - interSpeciesOffsprings.size()
        );

        // check that the overall ofssprings are equal to the population size
        uint64_t overallOffsprings = interSpeciesOffsprings.size();
        for (const auto &n : offspringsPerSpecies)
            overallOffsprings += n;

        ASSERT_EQUAL(overallOffsprings, opts.populationSize(), 
                     "to much or to less offsprings: %lu != %u",
                     overallOffsprings, opts.populationSize());

        // calculate the inspecies offsprings
        vector<shared_ptr<Organism>> inSpeciesOffsprings = population.createInSpeciesOffsprings(
            offspringsPerSpecies
        );
        
        // check that there arent to much in species offsprings
        ASSERT_TRUE(inSpeciesOffsprings.size() <= 
                    opts.populationSize() - interSpeciesOffsprings.size(),
                    "to much in species offsprings: %lu <= %lu",
                    inSpeciesOffsprings.size(),
                    opts.populationSize() - interSpeciesOffsprings.size());

        // sort the offsprings into species
        population.sortOffspringsIntoSpecies(interSpeciesOffsprings);
        population.sortOffspringsIntoSpecies(inSpeciesOffsprings);
        
        // finish the generation change by doing cleanups
        population.finishGenerationChange(lastGeneration);

        // check the number of Organisms 
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

        // check if we have a wining candidate 
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
    opts.mutateNeuronsPropability(0.0);
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
    opts.netActivationFunction(ActivationFunction::RELU);
    opts.weightMultiplier(1.0);
    opts.speciesSimilarityThreshold(5.0);
    opts.allowRecurentConnections(true);
    opts.ageingPropabilityOffset(0.0);
    opts.gradientMultiplier(0.0);
    opts.recombinateNeurons(false);
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

            
            //Phenotype phenotype2(best->getGenome(), opts);
            //Phenotype phenotype3(best->getGenome(), opts);
            //for (unsigned t = 0; t < trainingOutputs.size(); t++)
            //    printf("%f  =>  %f\n", trainingOutputs[t][0], phenotype2.run(trainingInputs[t])[0]);
            //
            //printf("maxDerivation: %f\n", phenotype3.maxDerivation(trainingInputs, trainingOutputs));

            if (r > nRuns / 1.5) {
                ASSERT_TRUE(avgVector(generations) < 40);
                ASSERT_TRUE(avgVector(nodes) < 5);
                ASSERT_TRUE(avgVector(connections) < 7);
                ASSERT_TRUE(avgVector(species) < 8);
            }
        }

    }

    return args;
}


TEST(binaryXORQualityTanhTest) {

    Opts opts;
    opts.netActivationFunction(ActivationFunction::TANH);

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
    opts.netActivationFunction(ActivationFunction::TANH);

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
                ASSERT_TRUE(avgVector(generations) < 25);
                ASSERT_TRUE(avgVector(nodes) < 7);
                ASSERT_TRUE(avgVector(connections) < 10);
                ASSERT_TRUE(avgVector(species) < 19);
            }
        }

    }

    return args;
}

TEST(binaryXORRandomActivationFunctionQualityTest) {

    // training data
    Opts opts;
    opts.randomActivationFunction(true);
    opts.mutateFunctionPropability(0.01);
    opts.speciesSimilarityThreshold(3.5);
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
                ASSERT_TRUE(avgVector(generations) < 25);
                ASSERT_TRUE(avgVector(nodes) < 7);
                ASSERT_TRUE(avgVector(connections) < 10);
                ASSERT_TRUE(avgVector(species) < 16);
            }
        }

    }

    return args;
}

class SinusFitnessCalculator: public FitnessCalculator {

    private:

        /* reference to the options of the whole population */
        Opts &opts;

        uint64_t organismIndex = 0;

        /* virtual fitness operator */
        virtual double operator () (const Genotype &genome) {

            organismIndex++;
            Phenotype phenotype(genome, opts);
            double x = 0.0;
            double error = 0.0;


            for (unsigned i = 0; i < 60; i++) {
                const double output = phenotype.run(vector<double>()).at(0);
                const double target = sin(x);
                error += pow(output - target, 2);
                x += 0.5;
            }

            error /= 60;
            return 1.0 / error;
        }

    public:

        /* constructor */
        SinusFitnessCalculator(Opts &opts) : opts(opts.freeze()) { }

        /* visualizes the fitness callculation */
        virtual void visualize(const Genotype &genome, const unsigned length, const unsigned timeSteps) {

            Phenotype phenotype(genome, opts);
            double x = 0.0;

            for (unsigned i = 0; i < timeSteps; i++) {
                const double output = phenotype.run(vector<double>()).at(0);
                const double target = sin(x);

                printf("|");
                for (unsigned n = 0; n < length; n++) {
                    const double curValue  = (n - length / 2.0) / (length / 2.0);
                    const double nextValue = ((n + 1) - length / 2.0) / (length / 2.0);

                    if (curValue < output && output < nextValue)
                        printf("#");
                    else if (curValue < target && target < nextValue)
                        printf("*");
                    else
                        printf(" ");
                }
                printf("|\n");

                x += 0.5;
            }
        }

};

TEST(sinusRecurencyTest) {

    Opts opts;
    opts.netActivationFunction(ActivationFunction::TANH);
    opts.weightMultiplier(1.0);
    opts.speciesSimilarityThreshold(5.0);
    opts.allowRecurentConnections(true);
    opts.ageingPropabilityOffset(0.0);
    opts.recombinateNeurons(false);
    opts.allowNeuronIntoBiasConnectionInsert(true);
    opts.shuffleTrainingData(false);
    //opts.removeNeuronMutationPropability(0.003);
    //opts.removeConnectionMutationPropability(0.005);

    // training data
    vector<vector<double>> trainingInputs  = {{}};
    vector<vector<double>> trainingOutputs = {{0}};
    SinusFitnessCalculator fitnessCalculator(opts);

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 1000;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts, &fitnessCalculator);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            best->calculateFittness(&fitnessCalculator);
            //fitnessCalculator.visualize(best->getGenome(), 100, 150);
            //
            //unsigned enabledGenes = 0;
            //for (const auto &c : best->getGenome().getConnections())
                //if (c.second.isEnabled())
                    //enabledGenes++;
            //
            //TEST_MSGD("[%u] Generation %3u, Nodes %3u, Connections: %3u, Species %3u, Fitness %F", r,
                      //i, (unsigned) best->getGenome().getNeurons().size(),
                      //enabledGenes, (unsigned) population.getSpecies().size(),
                      //1.0 / best->getFitness());
            //
            if (1.0 / best->getFitness() < 0.02)
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
                ASSERT_TRUE(avgVector(generations) < 40);
                ASSERT_TRUE(avgVector(nodes) < 5);
                ASSERT_TRUE(avgVector(connections) < 7);
                ASSERT_TRUE(avgVector(species) < 8);
            }
        }

    }

    return args;
}


class ParityFitnessCalculator: public FitnessCalculator {

    private:

        /* reference to the options of the whole population */
        Opts &opts;

        uint64_t organismIndex = 0;

        rand64_t *rand;

        /* virtual fitness operator */
        virtual double operator () (const Genotype &genome) {

            organismIndex++;
            Phenotype phenotype(genome, opts);
            double error = 0.0;

            double lastOutput = 0.0;
            for (unsigned i = 0; i < 1000; i++) {
                const double input  = (rand64(rand) % 2 == 0) ? -1.0 : 1.0;
                const double output = phenotype.run({input}).at(0);
                const double target = ((input <= 0 && lastOutput <= 0) ? -1.0 :
                                      ((input >  0 && lastOutput <= 0) ?  1.0 :
                                      ((input <= 0 && lastOutput  > 0) ?  1.0 :
                                      ((input >  0 && lastOutput  > 0) ? -1.0 : 0.0/0.0))));
                error += abs(output - target);
                lastOutput = output;
            }

            error /= 2000;
            return 1.0 / error;
        }

    public:

        /* constructor */
        ParityFitnessCalculator(Opts &opts) : opts(opts.freeze()), rand(new_rand64_t()) { }

        /* visualizes the fitness callculation */
        virtual void visualize(const Genotype &genome, const unsigned length, const unsigned timeSteps) {

            (void) length;
            Phenotype phenotype(genome, opts);

            double lastOutput = 0.0;
            for (unsigned i = 0; i < timeSteps; i++) {
                const double input  = (rand64(rand) % 2 == 0) ? -1.0 : 1.0;
                const double output = phenotype.run({input}).at(0);
                const double target = ((input < 0 && lastOutput < 0) ? -1.0 :
                                      ((input > 0 && lastOutput < 0) ?  1.0 :
                                      ((input < 0 && lastOutput > 0) ?  1.0 :
                                      ((input > 0 && lastOutput > 0) ? -1.0 : 0.0/0.0))));

                lastOutput = output;

                if (abs(target - output) < 0.25)
                    printf(" ");
                else if (abs(target - output) < 0.5)
                    printf("_");
                else if (abs(target - output) < 0.75)
                    printf("-");
                else if (abs(target - output) < 1.0)
                    printf("~");
                else if (abs(target - output) < 1.25)
                    printf("+");
                else if (abs(target - output) < 1.5)
                    printf("*");
                else if (abs(target - output) < 1.75)
                    printf("$");
                else if (abs(target - output) < 2.0)
                    printf("%%");
                else
                    printf("#");
            }
            printf("\n");
        }

};

TEST(ParityRecurencyTest) {

    Opts opts;
    opts.netActivationFunction(ActivationFunction::TANH);
    opts.weightMultiplier(0.1);
    opts.speciesSimilarityThreshold(1.0);
    opts.allowRecurentConnections(true);
    opts.ageingPropabilityOffset(0.0);
    opts.recombinateNeurons(false);
    opts.allowNeuronIntoBiasConnectionInsert(true);
    opts.shuffleTrainingData(false);
    opts.mutateNeuronsPropability(0.0);
    opts.mutationMaxPertubeWeightRangeStart(-8);
    opts.mutationMaxPertubeWeightRangeEnd(8);
    opts.addNeuronMutationPropability(0.001);
    opts.numThreads(15);

    // training data
    vector<vector<double>> trainingInputs  = {{1}};
    vector<vector<double>> trainingOutputs = {{0}};
    ParityFitnessCalculator fitnessCalculator(opts);

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 500;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts, &fitnessCalculator);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            best->calculateFittness(&fitnessCalculator);
            //fitnessCalculator.visualize(best->getGenome(), 100, 211);
            //
            //unsigned enabledGenes = 0;
            //for (const auto &c : best->getGenome().getConnections())
            //  if (c.second.isEnabled())
            //      enabledGenes++;
            //
            //TEST_MSGD("[%u] Generation %3u, Nodes %3u, Connections: %3u, Species %3u, Fitness %F", r,
            //        i, (unsigned) best->getGenome().getNeurons().size(),
            //        enabledGenes, (unsigned) population.getSpecies().size(),
            //        1.0 / best->getFitness());
            
            if (1.0 / best->getFitness() < 0.0002)
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
                ASSERT_TRUE(avgVector(generations) < 80);
                ASSERT_TRUE(avgVector(nodes) < 6);
                ASSERT_TRUE(avgVector(connections) < 15);
                ASSERT_TRUE(avgVector(species) < 50);
            }
        }

    }

    return args;
}


class MemoryFitnessCalculator: public FitnessCalculator {

    private:

        /* reference to the options of the whole population */
        Opts &opts;

        /* the number of signals to be processed by the net */
        unsigned nRuns; 
        
        /* the signal length */
        unsigned signalLength;
        
        /* random number generator */
        rand64_t *rand;

        /* virtual fitness operator */
        virtual double operator () (const Genotype &genome) {

            Phenotype phenotype(genome, opts);
            double error = 0.0;
            double dist  = 0.0;
            double last  = -1000;
            
            //double avg[2] = { 0.0, 0.0 };

            for (unsigned n = 0; n < nRuns; n++) {
                
                phenotype.initNet();
                double type   = ((n / 2) % 2) ? -1 : 1;
                double signes = (n % 2) ? -1 : 1;

                phenotype.run(vector<double>(1, type));

                for (unsigned i = 0; i < signalLength; i++)
                    phenotype.run(vector<double>(1, 0.0));

                const double output = phenotype.run(vector<double>(1, type * signes)).at(0);
                error += pow(signes - output, 2);

                if (last > -1000)
                    dist += abs(last - output);

                last = output;
            }

            if (dist < 0.1)
                return dist;

            return 1.0 / (sqrt(error / nRuns) + 0.000000000001);
        }

    public:

        /* return wether the problem was learned */
        bool learned(const Genotype &genome) {

            Phenotype phenotype(genome, opts);
            
            //double avg[2] = { 0.0, 0.0 };

            for (unsigned n = 0; n < nRuns; n++) {
                
                phenotype.initNet();
                double type   = ((n / 2) % 2) ? -1 : 1;
                double signes = (n % 2) ? -1 : 1;

                phenotype.run(vector<double>(1, type));

                for (unsigned i = 0; i < signalLength; i++)
                    phenotype.run(vector<double>(1, 0.0));

                if (abs(signes - phenotype.run(vector<double>(1, type * signes)).at(0)) >= 1)
                    return false;
            }

            return true;
        }

        /* visualizes the fitness callculation */
        virtual void visualize(const Genotype &genome) {

            Phenotype phenotype(genome, opts);
            
            //double avg[2] = { 0.0, 0.0 };

            for (unsigned n = 0; n < nRuns; n++) {
                
                phenotype.initNet();
                double type   = ((n / 2) % 2) ? -1 : 1;
                double signes = (n % 2) ? -1 : 1;

                phenotype.run(vector<double>(1, type));

                for (unsigned i = 0; i < signalLength; i++)
                    phenotype.run(vector<double>(1, 0.0));

                printf("[%2d, %2d] => %2f\n", int(type), int(signes*type), phenotype.run(vector<double>(1, type * signes)).at(0));
            }

        }

        /* constructor */
        MemoryFitnessCalculator(Opts &opts, unsigned nRuns = 4, unsigned signalLength = 4) : 
            opts(opts.freeze()), nRuns(nRuns), signalLength(signalLength) { }
};

TEST(MemoryRecurencyTest) {

    Opts opts;
    opts.netActivationFunction(ActivationFunction::TANH);
    //opts.weightMultiplier(1.0);
    opts.speciesSimilarityThreshold(5.0);
    opts.allowRecurentConnections(true);
    opts.ageingPropabilityOffset(0.0);
    //opts.recombinateNeurons(false);
    opts.allowNeuronIntoBiasConnectionInsert(true);
    opts.shuffleTrainingData(false);
    //opts.removeNeuronMutationPropability(0.003);
    //opts.removeConnectionMutationPropability(0.005);
    //opts.addNeuronMutationPropability(0.012);
    //opts.addConnectionMutationPropability(0.02);

    // training data
    vector<vector<double>> trainingInputs  = {{0}};
    vector<vector<double>> trainingOutputs = {{0}};
    MemoryFitnessCalculator fitnessCalculator(opts);

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 1000;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts, &fitnessCalculator);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            best->calculateFittness(&fitnessCalculator);
//            fitnessCalculator.visualize(best->getGenome());
//            
//            unsigned enabledGenes = 0;
//            for (const auto &c : best->getGenome().getConnections())
//              if (c.second.isEnabled())
//                  enabledGenes++;
//            
//            TEST_MSGD("[%u] Generation %3u, Nodes %3u, Connections: %3u, Species %3u, Fitness %F, Learned %s", r,
//                      i, (unsigned) best->getGenome().getNeurons().size(),
//                      enabledGenes, (unsigned) population.getSpecies().size(),
//                      1.0 / best->getFitness(),
//                      fitnessCalculator.learned(best->getGenome()) ? "true" : "false");
            
            if (fitnessCalculator.learned(best->getGenome()))
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
                ASSERT_TRUE(avgVector(generations) < 110);
                ASSERT_TRUE(avgVector(nodes) < 11);
                ASSERT_TRUE(avgVector(connections) < 20);
                ASSERT_TRUE(avgVector(species) < 15);
            }
        }

    }

    return args;
}


TEST(basicBinaryXORSpikingTest) {

    // training data
    Opts opts;
    opts.netActivationFunction(ActivationFunction::SPIKING);
    //opts.recombinateNeurons(false);
    opts.shuffleTrainingData(true);
    opts.speciesSimilarityThreshold(20);
    opts.resetNetBeforRun(true);
    //opts.mutateNeuronsPropability(0.0);

    opts.perturbeWeightRangeStart(-25);
    opts.perturbeWeightRangeEnd(25);
    opts.mutationSetWeightRangeStart(-25);
    opts.mutationSetWeightRangeEnd(25);
    opts.creationWeightRangeStart(-10);
    opts.creationWeightRangeEnd(10);


    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {1}, {1}, {0}};

    Population population(trainingInputs, trainingOutputs, opts);
    population.createInitialConnections();

    for (unsigned i = 0; i < 15000; i++) {
        uint64_t time = gettime_usec();

        // calculate the Organisms for reproduction
        vector<shared_ptr<Organism>> lastGeneration = population.prepareGenerationChange();

        // calculate the interspecies offsprings 
        // (add one offsprings more based on the propability given from the decimal part )
        //
        vector<shared_ptr<Organism>> interSpeciesOffsprings = population.createInterSpeciesOffsprings();
        
        // check that there are not to much inter species offsprings 
        ASSERT_TRUE(interSpeciesOffsprings.size() <
                    opts.populationSize() * opts.interSpeciesMatingPropability() + 1,
                    "to much inter species offsprings: %lu < %F",
                    interSpeciesOffsprings.size(),
                    opts.populationSize() * opts.interSpeciesMatingPropability() + 1);

        // calculate the number of offsprings per species 
        vector<unsigned> offspringsPerSpecies = population.calculateOffspringsPerSpecies(
            opts.populationSize() - interSpeciesOffsprings.size()
        );

        // check that the overall ofssprings are equal to the population size
        uint64_t overallOffsprings = interSpeciesOffsprings.size();
        for (const auto &n : offspringsPerSpecies)
            overallOffsprings += n;

        ASSERT_EQUAL(overallOffsprings, opts.populationSize(), 
                     "to much or to less offsprings: %lu != %u",
                     overallOffsprings, opts.populationSize());

        // calculate the inspecies offsprings
        vector<shared_ptr<Organism>> inSpeciesOffsprings = population.createInSpeciesOffsprings(
            offspringsPerSpecies
        );
        
        // check that there arent to much in species offsprings
        ASSERT_TRUE(inSpeciesOffsprings.size() <= 
                    opts.populationSize() - interSpeciesOffsprings.size(),
                    "to much in species offsprings: %lu <= %lu",
                    inSpeciesOffsprings.size(),
                    opts.populationSize() - interSpeciesOffsprings.size());

        // sort the offsprings into species
        population.sortOffspringsIntoSpecies(interSpeciesOffsprings);
        population.sortOffspringsIntoSpecies(inSpeciesOffsprings);
        
        // finish the generation change by doing cleanups
        population.finishGenerationChange(lastGeneration);

        // check the number of Organisms 
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

        // check if we have a wining candidate 
        shared_ptr<Organism> best = population.getBestOrganism();
        if (best->getFitness() == 1.0)
            break;

        unsigned enabledGenes = 0;
        for (const auto &c : best->getGenome().getConnections())
          if (c.second.isEnabled())
              enabledGenes++;
        
        TEST_MSGD("Generation %3u, Nodes %3u, Connections: %3u, Species %3u, Fitness %F",
                i, (unsigned) best->getGenome().getNeurons().size(),
                enabledGenes, (unsigned) population.getSpecies().size(),
                best->getFitness());

    }

    shared_ptr<Organism> best = population.getBestOrganism();
    Phenotype phenotype(best->getGenome(), opts);

    phenotype.saveAsSVG("basicBinaryXORTest.svg");
    //phenotype.printWeights();

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

TEST(binaryXORSpikingQualityTest) {

    // training data
    Opts opts;
    opts.netActivationFunction(ActivationFunction::SPIKING);
    //opts.recombinateNeurons(false);
    opts.shuffleTrainingData(true);
    opts.speciesSimilarityThreshold(20);
    opts.resetNetBeforRun(true);
    //opts.mutateNeuronsPropability(0.0);
    //opts.gradientMultiplier(0.0);

    opts.perturbeWeightRangeStart(-25);
    opts.perturbeWeightRangeEnd(25);
    opts.mutationSetWeightRangeStart(-25);
    opts.mutationSetWeightRangeEnd(25);
    opts.creationWeightRangeStart(-10);
    opts.creationWeightRangeEnd(10);

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

            if (phenotype.maxDerivation(trainingInputs, trainingOutputs) < 0.1)
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
                ASSERT_TRUE(avgVector(generations) < 70);
                ASSERT_TRUE(avgVector(nodes) < 5);
                ASSERT_TRUE(avgVector(connections) < 4);
                ASSERT_TRUE(avgVector(species) < 20);
            }
        }

    }

    return args;
}


TEST(binaryXORSpikingZeroStartConectionsQualityTest) {
    
    Opts opts;
    opts.mutateNeuronsPropability(0.0);
    opts.gradientMultiplier(0.0);
    opts.netActivationFunction(ActivationFunction::SPIKING);
    opts.recombinateNeurons(false);
    opts.shuffleTrainingData(true);
    opts.speciesSimilarityThreshold(20);
    opts.resetNetBeforRun(true);

    opts.perturbeWeightRangeStart(-25);
    opts.perturbeWeightRangeEnd(25);
    opts.mutationSetWeightRangeStart(-25);
    opts.mutationSetWeightRangeEnd(25);
    opts.creationWeightRangeStart(-10);
    opts.creationWeightRangeEnd(10);

    // training data
    vector<vector<double>> trainingInputs  = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingOutputs = {{0}, {1}, {1}, {0}};

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 400;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts);

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            Phenotype phenotype(best->getGenome(), opts);

            if (phenotype.maxDerivation(trainingInputs, trainingOutputs) < 0.1)
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
                ASSERT_TRUE(avgVector(generations) < 150);
                ASSERT_TRUE(avgVector(nodes) < 5);
                ASSERT_TRUE(avgVector(connections) < 4);
                ASSERT_TRUE(avgVector(species) < 25);
            }
        }

    }

    return args;
}


TEST(basicSpikingRecurencyTest) {
    
    Opts opts;
    opts.allowRecurentConnections(true);
    opts.ageingPropabilityOffset(0.0);
    opts.gradientMultiplier(0.0);

    opts.mutateNeuronsPropability(0.0);
    opts.netActivationFunction(ActivationFunction::SPIKING);
    opts.recombinateNeurons(false);
    opts.shuffleTrainingData(false);
    opts.speciesSimilarityThreshold(20);

    opts.perturbeWeightRangeStart(-25);
    opts.perturbeWeightRangeEnd(25);
    opts.mutationSetWeightRangeStart(-25);
    opts.mutationSetWeightRangeEnd(25);
    opts.creationWeightRangeStart(-10);
    opts.creationWeightRangeEnd(10);


    // training data
    vector<vector<double>> trainingInputs  = {{1}, {1}, {1}, {1}, {1}, {1}, {1}, {1}, {1}, {1}};
    vector<vector<double>> trainingOutputs = {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {1}};

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 500000;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            Phenotype phenotype(best->getGenome(), opts);

            if (phenotype.maxDerivation(trainingInputs, trainingOutputs) < 0.1)
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

            
            //Phenotype phenotype2(best->getGenome(), opts);
            //Phenotype phenotype3(best->getGenome(), opts);
            //for (unsigned t = 0; t < trainingOutputs.size(); t++)
            //    printf("%f  =>  %f\n", trainingOutputs[t][0], phenotype2.run(trainingInputs[t])[0]);
            //
            //printf("maxDerivation: %f\n", phenotype3.maxDerivation(trainingInputs, trainingOutputs));

            if (r > nRuns / 1.5) {
                ASSERT_TRUE(avgVector(generations) < 70);
                ASSERT_TRUE(avgVector(nodes) < 4);
                ASSERT_TRUE(avgVector(connections) < 4);
                ASSERT_TRUE(avgVector(species) < 5);
            }
        }

    }

    return args;
}


#if 0 // TODO find right parameters my be not sutable for spiking networks
class SinusSpikingFitnessCalculator: public FitnessCalculator {

    private:

        /* reference to the options of the whole population */
        Opts &opts;

        uint64_t organismIndex = 0;

        /* virtual fitness operator */
        virtual double operator () (const Genotype &genome) {

            organismIndex++;
            Phenotype phenotype(genome, opts);
            phenotype.initNet();
            double x = 0.0;
            double error = 0.0;


            for (unsigned i = 0; i < 30; i++) {
                const double output = phenotype.run(vector<double>()).at(0);
                const double target = (sin(x) + 1) / 2;
                error += pow(output - target, 2) * (30 - i);
                x += 0.5;
            }

            error /= 60;
            return 1.0 / error;
        }

    public:

        /* constructor */
        SinusSpikingFitnessCalculator(Opts &opts) : opts(opts.freeze()) { }

        /* visualizes the fitness callculation */
        virtual void visualize(const Genotype &genome, const unsigned length, const unsigned timeSteps) {

            Phenotype phenotype(genome, opts);
            phenotype.initNet();
            double x = 0.0;

            for (unsigned i = 0; i < timeSteps; i++) {
                const double output = phenotype.run(vector<double>()).at(0);
                const double target = (sin(x) + 1)/2;

                printf("|");
                for (unsigned n = 0; n < length; n++) {
                    const double curValue  = (n - length / 2.0) / (length / 2.0);
                    const double nextValue = ((n + 1) - length / 2.0) / (length / 2.0);

                    if (curValue < output && output < nextValue)
                        printf("#");
                    else if (curValue < target && target < nextValue)
                        printf("*");
                    else
                        printf(" ");
                }
                printf("|\n");

                x += 0.5;
            }
        }

};

TEST(sinusSpikingRecurencyTest) {

    Opts opts;
    opts.allowNeuronIntoBiasConnectionInsert(true);
    opts.perturbeWeightRangeStart(-25);
    opts.perturbeWeightRangeEnd(25);
    opts.mutationSetWeightRangeStart(-25);
    opts.mutationSetWeightRangeEnd(25);
    opts.creationWeightRangeStart(-10);
    opts.creationWeightRangeEnd(10);
    opts.speciesSimilarityThreshold(20);

    opts.allowRecurentConnections(true);
    opts.netActivationFunction(ActivationFunction::SPIKING);
    opts.shuffleTrainingData(false);
    opts.spikingNumSimulationStepsPerOutoput(16);

    // training data
    vector<vector<double>> trainingInputs  = {{}};
    vector<vector<double>> trainingOutputs = {{0}};
    SinusSpikingFitnessCalculator fitnessCalculator(opts);

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 5000;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts, &fitnessCalculator);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            best->calculateFittness(&fitnessCalculator);
            fitnessCalculator.visualize(best->getGenome(), 100, 30);
            
            unsigned enabledGenes = 0;
            for (const auto &c : best->getGenome().getConnections())
                if (c.second.isEnabled())
                    enabledGenes++;
            
            TEST_MSGD("[%u] Generation %3u, Nodes %3u, Connections: %3u, Species %3u, Fitness %F", r,
                      i, (unsigned) best->getGenome().getNeurons().size(),
                      enabledGenes, (unsigned) population.getSpecies().size(),
                      1.0 / best->getFitness());
            
            if (1.0 / best->getFitness() < 0.02)
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
                ASSERT_TRUE(avgVector(generations) < 40);
                ASSERT_TRUE(avgVector(nodes) < 5);
                ASSERT_TRUE(avgVector(connections) < 7);
                ASSERT_TRUE(avgVector(species) < 8);
            }
        }

    }

    return args;
}
#endif


class ParitySpikingFitnessCalculator: public FitnessCalculator {

    private:

        /* reference to the options of the whole population */
        Opts &opts;

        uint64_t organismIndex = 0;

        rand64_t *rand;

        /* virtual fitness operator */
        virtual double operator () (const Genotype &genome) {

            organismIndex++;
            Phenotype phenotype(genome, opts);
            phenotype.initNet();
            double error = 0.0;

            double lastOutput = 0.0;
            int numOnes = 0;
            int sameOutputCounter = 0;
            bool learned = true;
            for (unsigned i = 0; i < 300; i++) {
                const double input  = rand64(rand) % 2;
                const double output = phenotype.run({input}).at(0);
                const double target = ((input <= 0 && lastOutput <= 0) ? 0.0 :
                                      ((input >  0 && lastOutput <= 0) ? 1.0 :
                                      ((input <= 0 && lastOutput  > 0) ? 1.0 :
                                      ((input >  0 && lastOutput  > 0) ? 0.0 : 0.0/0.0))));

                if (target > 0.5)
                    numOnes++;

                if ((lastOutput >  0 && output >  0) ||
                    (lastOutput <= 0 && output <= 0))
                    sameOutputCounter++;

                if ((lastOutput > 0 && output <= 0)  ||
                     (lastOutput <= 0 && output > 0))
                    sameOutputCounter = 0;

                if (sameOutputCounter > 30)
                    error += 1;

                if (abs(output - target) >= 0.5)
                    learned = false;

                error += abs(output - target) * (300 - i);
                lastOutput = (output <= 0.5) ? 0 : 1;

            }

            if (learned)
                error /= numOnes;

            return 1.0 / (error + 0.00000001);
        }

    public:

        /* constructor */
        ParitySpikingFitnessCalculator(Opts &opts) : opts(opts.freeze()), rand(new_rand64_t()) { }

        /* virtual fitness operator */
        bool learned(const Genotype &genome) {

            organismIndex++;
            Phenotype phenotype(genome, opts);
            phenotype.initNet();

            double lastOutput = 0.0;
            for (unsigned i = 0; i < 300; i++) {
                const double input  = rand64(rand) % 2;
                const double output = phenotype.run({input}).at(0);
                const double target = ((input <= 0 && lastOutput <= 0) ? 0.0 :
                                      ((input >  0 && lastOutput <= 0) ? 1.0 :
                                      ((input <= 0 && lastOutput  > 0) ? 1.0 :
                                      ((input >  0 && lastOutput  > 0) ? 0.0 : 0.0/0.0))));

                if (abs(output - target) >= 0.5)
                    return false;

                lastOutput = (output <= 0.5) ? 0 : 1;
            }

            return true;
        }

        /* visualizes the fitness callculation */
        virtual void visualize(const Genotype &genome, const unsigned length, const unsigned timeSteps) {

            (void) length;
            Phenotype phenotype(genome, opts);
            phenotype.initNet();

            double lastOutput = 0.0;
            vector<double> outputs, targets;
            for (unsigned i = 0; i < timeSteps; i++) {
                const double input  = rand64(rand) % 2;
                const double output = phenotype.run({input}).at(0);
                const double target = ((input <= 0 && lastOutput <= 0) ? 0.0 :
                                      ((input >  0 && lastOutput <= 0) ? 1.0 :
                                      ((input <= 0 && lastOutput >  0) ? 1.0 :
                                      ((input >  0 && lastOutput >  0) ? 0.0 : 0.0/0.0))));

                lastOutput = (output <= 0.5) ? 0 : 1;
                outputs.push_back(output);
                targets.push_back(target);
            }
            printf("Sol: ");
            for (unsigned i = 0; i < timeSteps; i++) {
                if (targets[i] < 0.125)
                    printf(" ");
                else if (targets[i] < 0.25)
                    printf("_");
                else if (targets[i] < 0.375)
                    printf("-");
                else if (targets[i] < 0.5)
                    printf("~");
                else if (targets[i] < 0.625)
                    printf("+");
                else if (targets[i] < 0.75)
                    printf("*");
                else if (targets[i] < 0.875)
                    printf("$");
                else if (targets[i] < 1.0)
                    printf("%%");
                else
                    printf("#");
            }

            printf("\nOut: ");
            for (unsigned i = 0; i < timeSteps; i++) {
                if (outputs[i] < 0.125)
                    printf(" ");
                else if (outputs[i] < 0.25)
                    printf("_");
                else if (outputs[i] < 0.375)
                    printf("-");
                else if (outputs[i] < 0.5)
                    printf("~");
                else if (outputs[i] < 0.625)
                    printf("+");
                else if (outputs[i] < 0.75)
                    printf("*");
                else if (outputs[i] < 0.875)
                    printf("$");
                else if (outputs[i] < 1.0)
                    printf("%%");
                else
                    printf("#");
            }

            printf("\nErr: ");
            for (unsigned i = 0; i < timeSteps; i++) {
                if (abs(targets[i] - outputs[i]) < 0.125)
                    printf(" ");
                else if (abs(targets[i] - outputs[i]) < 0.25)
                    printf("_");
                else if (abs(targets[i] - outputs[i]) < 0.375)
                    printf("-");
                else if (abs(targets[i] - outputs[i]) < 0.5)
                    printf("~");
                else if (abs(targets[i] - outputs[i]) < 0.625)
                    printf("+");
                else if (abs(targets[i] - outputs[i]) < 0.75)
                    printf("*");
                else if (abs(targets[i] - outputs[i]) < 0.875)
                    printf("$");
                else if (abs(targets[i] - outputs[i]) < 1.0)
                    printf("%%");
                else
                    printf("#");
            }
            printf("\n");
        }

};

TEST(ParitySpikingRecurencyTest) {

    Opts opts;
    opts.addNeuronMutationPropability(0.01);

    opts.allowRecurentConnections(true);
    opts.netActivationFunction(ActivationFunction::SPIKING);
    opts.numThreads(10);
    opts.allowNeuronIntoBiasConnectionInsert(true);
    //opts.ageingPropabilityOffset(0.0);
    opts.shuffleTrainingData(false);
    opts.speciesSimilarityThreshold(10);

    opts.perturbeWeightRangeStart(-35);
    opts.perturbeWeightRangeEnd(35);
    opts.mutationSetWeightRangeStart(-35);
    opts.mutationSetWeightRangeEnd(35);
    opts.creationWeightRangeStart(-20);
    opts.creationWeightRangeEnd(20);
    opts.spikingNumSimulationStepsPerOutoput(1);


    // training data
    vector<vector<double>> trainingInputs  = {{1}};
    vector<vector<double>> trainingOutputs = {{0}};
    ParitySpikingFitnessCalculator fitnessCalculator(opts);

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 5000;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts, &fitnessCalculator);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            best->calculateFittness(&fitnessCalculator);
//            fitnessCalculator.visualize(best->getGenome(), 100, 200);
//            
//            unsigned enabledGenes = 0;
//            for (const auto &c : best->getGenome().getConnections())
//              if (c.second.isEnabled())
//                  enabledGenes++;
//            
//            TEST_MSGD("[%u] Generation %3u, Nodes %3u, Connections: %3u, Species %3u, Fitness %F, Learned %s", r,
//                    i, (unsigned) best->getGenome().getNeurons().size(),
//                    enabledGenes, (unsigned) population.getSpecies().size(),
//                    1.0 / best->getFitness(),
//                    fitnessCalculator.learned(best->getGenome()) ? "true" : "false");
            
            if (fitnessCalculator.learned(best->getGenome()))
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
                ASSERT_TRUE(avgVector(generations) < 150);
                ASSERT_TRUE(avgVector(nodes) < 5);
                ASSERT_TRUE(avgVector(connections) < 6);
                ASSERT_TRUE(avgVector(species) < 40);
            }
        }

    }

    return args;
}


class SpikingMemoryFitnessCalculator: public FitnessCalculator {

    private:

        /* reference to the options of the whole population */
        Opts &opts;

        /* the number of signals to be processed by the net */
        unsigned nRuns; 
        
        /* the signal length */
        unsigned signalLength;
        
        /* random number generator */
        rand64_t *rand;

        /* virtual fitness operator */
        virtual double operator () (const Genotype &genome) {

            Phenotype phenotype(genome, opts);
            double error = 0.0;
            double last  = -1000;
            double dist  = 0.0;

            double avg[2] = { 0.0, 0.0 };
            vector<double> outputs(nRuns, 0.0);
            for (unsigned n = 0; n < nRuns; n++) {
                
                phenotype.initNet();
                double type   = ((n / 2) % 2) ? -1 : 1;
                double signes = (n % 2) ? -1 : 1;

                for (int x = 0; x < 16; x++)
                    phenotype.runSinglePass(vector<double>(1, type));

                for (unsigned i = 0; i < signalLength; i++)
                    for (int x = 0; x < 16; x++)
                        phenotype.runSinglePass(vector<double>(1, 0.0));

                const double output = phenotype.runSpiking(vector<double>(1, type * signes)).at(0);
                avg[n % 2] += output;
                outputs[n] = output;

                if (last > -1000)
                    dist += abs(last - output);

                last = output;
            }

            if (dist < 0.000000001) 
                return dist + 0.000000000001;

            avg[0] /= nRuns / 2;
            avg[1] /= nRuns / 2;
            for (unsigned n = 0; n < nRuns; n++) {
                error += pow(avg[n % 2] - outputs[n], 2);
                double err[2] = { abs(outputs[n] - avg[0]), abs(outputs[n] - avg[1]) };

                if (err[n % 2] >= err[(n + 1) % 2])
                    error += pow(err[0] - err[1], 2);
            }

            return pow(avg[0] - avg[1], 2) + 1.0 / (sqrt(error / nRuns) + 0.001);
        }

    public:

        /* return wether the problem was learned */
        bool learned(const Genotype &genome) {

            Phenotype phenotype(genome, opts);

            double avg[2] = { 0.0, 0.0 };
            vector<double> outputs(nRuns, 0.0);
            for (unsigned n = 0; n < nRuns; n++) {
                
                phenotype.initNet();
                double type   = ((n / 2) % 2) ? -1 : 1;
                double signes = (n % 2) ? -1 : 1;

                for (int x = 0; x < 16; x++)
                    phenotype.runSinglePass(vector<double>(1, type));

                for (unsigned i = 0; i < signalLength; i++)
                    for (int x = 0; x < 16; x++)
                        phenotype.runSinglePass(vector<double>(1, 0.0));

                const double output = phenotype.runSpiking(vector<double>(1, type * signes)).at(0);
                avg[n % 2] += output;
                outputs[n] = output;
            }

            avg[0] /= nRuns / 2;
            avg[1] /= nRuns / 2;
            for (unsigned n = 0; n < nRuns; n++) {
                double err[2] = { abs(outputs[n] - avg[0]), abs(outputs[n] - avg[1]) };

                if (err[n % 2] >= err[(n + 1) % 2])
                    return false;
            }

            return true;
        }

        /* visualizes the fitness callculation */
        virtual void visualize(const Genotype &genome) {

            Phenotype phenotype(genome, opts);
            
            //double avg[2] = { 0.0, 0.0 };

            for (unsigned n = 0; n < nRuns; n++) {
                
                phenotype.initNet();
                double type   = ((n / 2) % 2) ? -1 : 1;
                double signes = (n % 2) ? -1 : 1;

                for (int x = 0; x < 16; x++)
                    phenotype.runSinglePass(vector<double>(1, type));

                for (unsigned i = 0; i < signalLength; i++)
                    for (int x = 0; x < 16; x++)
                        phenotype.runSinglePass(vector<double>(1, 0.0));

                printf(
                    "[%2d, %2d] => %2f\n", 
                    int(type), 
                    int(signes*type), 
                    phenotype.runSpiking(vector<double>(1, type * signes)).at(0)
                );
            }

        }

        /* constructor */
        SpikingMemoryFitnessCalculator(Opts &opts, unsigned nRuns = 4, unsigned signalLength = 4) : 
            opts(opts.freeze()), nRuns(nRuns), signalLength(signalLength) { }
};

TEST(SpikingMemoryRecurencyTest) {

    Opts opts;
    opts.netActivationFunction(ActivationFunction::SPIKING);
    opts.spikeOutputEncoding(Opts::SpikeCoding::TIME);
    opts.spikingMaxOutputSiumlationSteps(128);
    opts.layeredNetwork(false);
    //opts.outputActivationFunction(ActivationFunction::SIGMOID);
    //opts.weightMultiplier(1.0);
    opts.speciesSimilarityThreshold(5.0);
    opts.allowRecurentConnections(true);
    opts.ageingPropabilityOffset(0.0);
    //opts.recombinateNeurons(false);
    opts.allowNeuronIntoBiasConnectionInsert(true);
    opts.shuffleTrainingData(false);
    //opts.removeNeuronMutationPropability(0.003);
    //opts.removeConnectionMutationPropability(0.005);
    //opts.addNeuronMutationPropability(0.24);
    //opts.addConnectionMutationPropability(0.4);
    opts.perturbeWeightRangeStart(-25);
    opts.perturbeWeightRangeEnd(25);
    opts.mutationSetWeightRangeStart(-25);
    opts.mutationSetWeightRangeEnd(25);
    opts.creationWeightRangeStart(-10);
    opts.creationWeightRangeEnd(10);
    opts.numThreads(15);

    // training data
    vector<vector<double>> trainingInputs  = {{0}};
    vector<vector<double>> trainingOutputs = {{0}};
    SpikingMemoryFitnessCalculator fitnessCalculator(opts);

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 1000;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts, &fitnessCalculator);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            best->calculateFittness(&fitnessCalculator);
//            fitnessCalculator.visualize(best->getGenome());
//            
//            unsigned enabledGenes = 0;
//            for (const auto &c : best->getGenome().getConnections())
//              if (c.second.isEnabled())
//                  enabledGenes++;
//            
//            TEST_MSGD("[%u] Generation %3u, Nodes %3u, Connections: %3u, Species %3u, Fitness %F, Learned %s", r,
//                      i, (unsigned) best->getGenome().getNeurons().size(),
//                      enabledGenes, (unsigned) population.getSpecies().size(),
//                      1.0 / best->getFitness(),
//                      fitnessCalculator.learned(best->getGenome()) ? "true" : "false");
            
            if (fitnessCalculator.learned(best->getGenome()))
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
                ASSERT_TRUE(avgVector(generations) < 40);
                ASSERT_TRUE(avgVector(nodes) < 5);
                ASSERT_TRUE(avgVector(connections) < 7);
                ASSERT_TRUE(avgVector(species) < 8);
            }
        }

    }

    return args;
}

class BinaryClassificationFitnessCalculator: public FitnessCalculator {

    private:

        /* reference to the options of the whole population */
        Opts &opts;

        /* the number of signals to be processed by the net */
        unsigned nRuns; 
        
        /* the signal length */
        unsigned signalLength;
        
        /* random number generator */
        rand64_t *rand;

        /* virtual fitness operator */
        virtual double operator () (const Genotype &genome) {
            return classificationRate(genome);
        }

    public:

        /* return wether the problem was learned */
        bool learned(const Genotype &genome) {
            return classificationRate(genome) == 1.0;
        }

        /* visualizes the fitness callculation */
        double classificationRate(const Genotype &genome) {

            Phenotype phenotype(genome, opts);
            double error = 0.0;
            double stepsize = 0.1;

            double avg[2] = { 0.0, 0.0 };
            vector<double> outputs(nRuns, 0.0);
            for (unsigned n = 0; n < nRuns; n++) {

                double x = rand_range_d64(rand, 0, 6.283185307179586);
                phenotype.initNet();

                for (unsigned i = 0; i < signalLength; i++) {
                    double signal[2] = { sin(x) + rand_range_d64(rand, -1.0, 1.0), double(sgn(sin(x))) + rand_range_d64(rand, -1.0, 1.0) };
                    avg[n % 2] += phenotype.run(vector<double>(1, signal[n % 2])).at(0);
                    x += stepsize;
                }
            }

            avg[0] /= (nRuns/2) * signalLength;
            avg[1] /= (nRuns/2) * signalLength;

            for (unsigned n = 0; n < nRuns; n++) {

                double curAVG = 0.0;
                double x = rand_range_d64(rand, 0, 6.283185307179586);
                phenotype.initNet();


                for (unsigned i = 0; i < signalLength; i++) {
                    double signal[2] = { sin(x) + rand_range_d64(rand, -1.0, 1.0), double(sgn(sin(x))) + rand_range_d64(rand, -1.0, 1.0) };
                    curAVG += phenotype.run(vector<double>(1, signal[n % 2])).at(0);
                    x += stepsize;
                }
                curAVG /= signalLength;
                double err[2] = { abs(curAVG - avg[0]), abs(curAVG - avg[1]) };

                if (err[n % 2] >= err[(n + 1) % 2])
                    error += 1;
            }

            if (error == 0.0 && nRuns < 4096) {
                nRuns *= 2;   
                return classificationRate(genome);
            }
            return 1 - error / nRuns;
        }

        /* constructor */
        BinaryClassificationFitnessCalculator(Opts &opts, unsigned nRuns = 64, unsigned signalLength = 64) : 
            opts(opts.freeze()), nRuns(nRuns), signalLength(signalLength) { 
            
            rand = new_rand64_t();
        }
};

TEST(BinaryClassificationRecurencyTest) {

    Opts opts;
    opts.netActivationFunction(ActivationFunction::TANH);
    //opts.speciesSimilarityThreshold(5.0);
    opts.allowRecurentConnections(true);
    opts.alwaysRecalculateFitness(true);
    opts.ageingPropabilityOffset(0.0);
    opts.allowNeuronIntoBiasConnectionInsert(true);
    opts.shuffleTrainingData(false);
    opts.numThreads(15);

    // training data
    vector<vector<double>> trainingInputs  = {{0}};
    vector<vector<double>> trainingOutputs = {{0}};
    BinaryClassificationFitnessCalculator fitnessCalculator(opts);

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 1000;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts, &fitnessCalculator);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            best->calculateFittness(&fitnessCalculator);
//            fitnessCalculator.visualize(best->getGenome());
//            
//            unsigned enabledGenes = 0;
//            for (const auto &c : best->getGenome().getConnections())
//              if (c.second.isEnabled())
//                  enabledGenes++;
//            
//            TEST_MSGD("[%u] Generation %3u, Nodes %3u, Connections: %3u, Species %3u, Fitness %F, Classification %f", r,
//                      i, (unsigned) best->getGenome().getNeurons().size(),
//                      enabledGenes, (unsigned) population.getSpecies().size(),
//                      best->getFitness(),
//                      fitnessCalculator.classificationRate(best->getGenome()));
            
            if (best->getFitness() > 0.95)
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
                ASSERT_TRUE(avgVector(generations) < 40);
                ASSERT_TRUE(avgVector(nodes) < 5);
                ASSERT_TRUE(avgVector(connections) < 7);
                ASSERT_TRUE(avgVector(species) < 8);
            }
        }

    }

    return args;
}

TEST(BinaryClassificationSpikingRecurencyTest) {

    Opts opts;
    opts.netActivationFunction(ActivationFunction::SPIKING);
    opts.layeredNetwork(false);
    opts.speciesSimilarityThreshold(5.0);
    opts.allowRecurentConnections(true);
    opts.alwaysRecalculateFitness(true);
    opts.ageingPropabilityOffset(0.0);
    opts.allowNeuronIntoBiasConnectionInsert(true);
    opts.shuffleTrainingData(false);
    opts.numThreads(15);


    opts.perturbeWeightRangeStart(-25);
    opts.perturbeWeightRangeEnd(25);
    opts.mutationSetWeightRangeStart(-25);
    opts.mutationSetWeightRangeEnd(25);
    opts.creationWeightRangeStart(-10);
    opts.creationWeightRangeEnd(10);


    // training data
    vector<vector<double>> trainingInputs  = {{0}};
    vector<vector<double>> trainingOutputs = {{0}};
    BinaryClassificationFitnessCalculator fitnessCalculator(opts);

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 1000;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts, &fitnessCalculator);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            best->calculateFittness(&fitnessCalculator);
//            fitnessCalculator.visualize(best->getGenome());
//            
//            unsigned enabledGenes = 0;
//            for (const auto &c : best->getGenome().getConnections())
//              if (c.second.isEnabled())
//                  enabledGenes++;
//            
//            TEST_MSGD("[%u] Generation %3u, Nodes %3u, Connections: %3u, Species %3u, Fitness %F, Classification %f", r,
//                      i, (unsigned) best->getGenome().getNeurons().size(),
//                      enabledGenes, (unsigned) population.getSpecies().size(),
//                      best->getFitness(),
//                      fitnessCalculator.classificationRate(best->getGenome()));
            
            if (best->getFitness() > 0.95)
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
                ASSERT_TRUE(avgVector(generations) < 40);
                ASSERT_TRUE(avgVector(nodes) < 5);
                ASSERT_TRUE(avgVector(connections) < 7);
                ASSERT_TRUE(avgVector(species) < 8);
            }
        }

    }

    return args;
}

class FourSignalsClassificationFitnessCalculator: public FitnessCalculator {

    private:

        /* reference to the options of the whole population */
        Opts &opts;

        /* the number of signals to be processed by the net */
        unsigned nRuns; 
        
        /* the signal length */
        unsigned signalLength;
        
        /* random number generator */
        rand64_t *rand;

        double avg[4];

        /* virtual fitness operator */
        virtual double operator () (const Genotype &genome) {
            return classificationRate(genome);
        }

    public:

        /* return wether the problem was learned */
        bool learned(const Genotype &genome) {
            return classificationRate(genome) == 1.0;
        }

        /* visualizes the fitness callculation */
        virtual void visualize(const Genotype &) {
            
            printf("avg1: %f, avg2: %f, avg3: %f, avg4: %f\n", avg[0], avg[1], avg[2], avg[3]);
        }

        /* visualizes the fitness callculation */
        double classificationRate(const Genotype &genome) {

            Phenotype phenotype(genome, opts);
            double error = 0.0;
            double stepsize = 0.1;

            double avg[4] = { 0.0, 0.0, 0.0, 0.0 };
            vector<double> outputs(nRuns, 0.0);
            for (unsigned n = 0; n < nRuns; n++) {

                double x = rand_range_d64(rand, 0, 6.283185307179586);
                phenotype.initNet();

                for (unsigned i = 0; i < signalLength; i++) {
                    double signal[4] = { 
                        sin(x) + rand_range_d64(rand, -0.5, 0.5), 
                        double(sgn(sin(x))) + rand_range_d64(rand, -0.5, 0.5),
                        sin(x + sin(2*x)) + rand_range_d64(rand, -0.5, 0.5),
                        (sin(x) + sin(2*x) + sin(3*x) + sin(4*x) + sin(5*x) + sin(6*x) + sin(7*x) + sin(8*x)) / 8 + rand_range_d64(rand, -0.5, 0.5)
                    };

                    avg[n % 4] += phenotype.run(vector<double>(1, signal[n % 4])).at(0);
                    x += stepsize;
                }
            }

            avg[0] /= (nRuns/4) * signalLength;
            avg[1] /= (nRuns/4) * signalLength;
            avg[2] /= (nRuns/4) * signalLength;
            avg[3] /= (nRuns/4) * signalLength;

            for (unsigned n = 0; n < nRuns; n++) {

                double curAVG = 0.0;
                double x = rand_range_d64(rand, 0, 6.283185307179586);
                phenotype.initNet();


                for (unsigned i = 0; i < signalLength; i++) {
                    double signal[4] = { 
                        sin(x) + rand_range_d64(rand, -0.5, 0.5), 
                        double(sgn(sin(x))) + rand_range_d64(rand, -0.5, 0.5),
                        sin(x + sin(2*x)) + rand_range_d64(rand, -0.5, 0.5),
                        (sin(x) + sin(2*x) + sin(3*x) + sin(4*x) + sin(5*x) + sin(6*x) + sin(7*x) + sin(8*x)) / 8 + rand_range_d64(rand, -0.5, 0.5)
                    };
                    curAVG += phenotype.run(vector<double>(1, signal[n % 4])).at(0);
                    x += stepsize;
                }
                curAVG /= signalLength;
                double err[4] = { 
                    abs(curAVG - avg[0]), 
                    abs(curAVG - avg[1]), 
                    abs(curAVG - avg[2]), 
                    abs(curAVG - avg[3]) 
                };

                if (err[n % 4] >= err[(n + 1) % 4] || 
                    err[n % 4] >= err[(n + 2) % 4] || 
                    err[n % 4] >= err[(n + 3) % 4])
                    error += 1;
            }

            if (error == 0.0 && nRuns < 256) {
                nRuns *= 2;   
                return classificationRate(genome);
            }

            this->avg[0] = avg[0];
            this->avg[1] = avg[1];
            this->avg[2] = avg[2];
            this->avg[3] = avg[3];
            return 1 - error / nRuns;
        }

        /* constructor */
        FourSignalsClassificationFitnessCalculator(Opts &opts, unsigned nRuns = 64, unsigned signalLength = 64) : 
            opts(opts.freeze()), nRuns(nRuns), signalLength(signalLength) { 
            
            rand = new_rand64_t();
        }
};

TEST(FourSignalsClassificationRecurencyTest) {

    Opts opts;
    opts.netActivationFunction(ActivationFunction::TANH);
    //opts.speciesSimilarityThreshold(5.0);
    opts.allowRecurentConnections(true);
    opts.alwaysRecalculateFitness(true);
    opts.ageingPropabilityOffset(0.0);
    opts.allowNeuronIntoBiasConnectionInsert(true);
    opts.shuffleTrainingData(false);
    opts.numThreads(15);

    // training data
    vector<vector<double>> trainingInputs  = {{0}};
    vector<vector<double>> trainingOutputs = {{0}};
    FourSignalsClassificationFitnessCalculator fitnessCalculator(opts);

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 1000;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts, &fitnessCalculator);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            best->calculateFittness(&fitnessCalculator);
//            fitnessCalculator.visualize(best->getGenome());
//            
//            unsigned enabledGenes = 0;
//            for (const auto &c : best->getGenome().getConnections())
//              if (c.second.isEnabled())
//                  enabledGenes++;
//            
//            TEST_MSGD("[%u] Generation %3u, Nodes %3u, Connections: %3u, Species %3u, Fitness %F, Classification %f", r,
//                      i, (unsigned) best->getGenome().getNeurons().size(),
//                      enabledGenes, (unsigned) population.getSpecies().size(),
//                      best->getFitness(),
//                      fitnessCalculator.classificationRate(best->getGenome()));
            
            if (best->getFitness() > 0.98)
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
                ASSERT_TRUE(avgVector(generations) < 40);
                ASSERT_TRUE(avgVector(nodes) < 5);
                ASSERT_TRUE(avgVector(connections) < 7);
                ASSERT_TRUE(avgVector(species) < 8);
            }
        }

    }

    return args;
}

TEST(FourSignalsSpikingClassificationRecurencyTest) {

    Opts opts;
    opts.netActivationFunction(ActivationFunction::SPIKING);
    opts.layeredNetwork(false);
    opts.speciesSimilarityThreshold(15.0);
    opts.allowRecurentConnections(true);
    opts.alwaysRecalculateFitness(true);
    opts.ageingPropabilityOffset(0.0);
    opts.allowNeuronIntoBiasConnectionInsert(true);
    opts.shuffleTrainingData(false);
    opts.numThreads(15);


    opts.perturbeWeightRangeStart(-25);
    opts.perturbeWeightRangeEnd(25);
    opts.mutationSetWeightRangeStart(-25);
    opts.mutationSetWeightRangeEnd(25);
    opts.creationWeightRangeStart(-10);
    opts.creationWeightRangeEnd(10);

    // training data
    vector<vector<double>> trainingInputs  = {{0}};
    vector<vector<double>> trainingOutputs = {{0}};
    FourSignalsClassificationFitnessCalculator fitnessCalculator(opts);

    vector<unsigned> generations;
    vector<unsigned> nodes;
    vector<unsigned> connections;
    vector<unsigned> species;

    const unsigned nRuns = 1000;
    const unsigned nGenerations = 1000;

    for (unsigned r = 0; r < nRuns; r++) {

        Population population(trainingInputs, trainingOutputs, opts, &fitnessCalculator);
        population.createInitialConnections();

        unsigned i;
        for (i = 0; i < nGenerations; i++) {
            population.calcNextGeneration();

            shared_ptr<Organism> best = population.getBestOrganism();
            best->calculateFittness(&fitnessCalculator);
//            fitnessCalculator.visualize(best->getGenome());
//            
//            unsigned enabledGenes = 0;
//            for (const auto &c : best->getGenome().getConnections())
//              if (c.second.isEnabled())
//                  enabledGenes++;
//            
//            TEST_MSGD("[%u] Generation %3u, Nodes %3u, Connections: %3u, Species %3u, Fitness %F, Classification %f", r,
//                      i, (unsigned) best->getGenome().getNeurons().size(),
//                      enabledGenes, (unsigned) population.getSpecies().size(),
//                      best->getFitness(),
//                      fitnessCalculator.classificationRate(best->getGenome()));
            
            if (best->getFitness() > 0.90)
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
                ASSERT_TRUE(avgVector(generations) < 40);
                ASSERT_TRUE(avgVector(nodes) < 5);
                ASSERT_TRUE(avgVector(connections) < 7);
                ASSERT_TRUE(avgVector(species) < 8);
            }
        }

    }

    return args;
}

