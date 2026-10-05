#include "Population.h"
#include "Organism.h"
#include "Opts.h"
#include "ParallelWorker.h"
#include <memory>
#include <algorithm>

using std::shared_ptr;
using std::make_shared;
using namespace NEAT;

/* abstract destructor */
SpeciesInspector::~SpeciesInspector() { };

/* global random value */
rand64_t *Population::rand = new_rand64_t();

/* create a new Population */
Population::Population(
    vector<vector<double>> inputs, 
    vector<vector<double>> outputs,
    Opts &opts,
    FitnessCalculator *fitnessCalculator
) : 
    opts(opts.freeze()),
    geneHistory(opts),
    fitnessCalculator(fitnessCalculator) {

    this->inputs  = inputs;
    this->outputs = outputs;
    this->bestFitness = 0;
    this->lastImproveGeneration = GenerationCounter::get();

    this->species.push_back(make_shared<Species>(this->opts, this->geneHistory));

    for (unsigned i = 0; i < opts.populationSize(); i++) {
        this->species.back()->add(make_shared<Organism>(
            inputs[0].size(), 
            outputs[0].size(), 
            this->opts,
            this->geneHistory
        ));
    }
}

/* create a new Population */
Population::Population(FitnessCalculator *fitnessCalculator, Opts &opts) : 
    opts(opts.freeze()),
    geneHistory(opts),
    fitnessCalculator(fitnessCalculator) {

    this->inputs  = vector<vector<double>>();
    this->outputs = vector<vector<double>>();
    this->bestFitness = 0;
    this->lastImproveGeneration = GenerationCounter::get();

    this->species.push_back(make_shared<Species>(this->opts, this->geneHistory));

    if (this->opts.numInputNeurons() > 0 && this->opts.numOutputNeurons() > 0) {
        for (unsigned i = 0; i < opts.populationSize(); i++) {
            this->species.back()->add(make_shared<Organism>(
                this->opts.numInputNeurons(),
                this->opts.numOutputNeurons(),
                this->opts,
                this->geneHistory
            ));
        }
    } else {
        for (unsigned i = 0; i < opts.populationSize(); i++) {
            this->species.back()->add(make_shared<Organism>(
                this->opts,
                this->geneHistory
            ));
        }
    }
}

/* sorts the Organisms and species as removing the deaths 
 * returns the organisms left for reproduction
 */
vector<shared_ptr<Organism>> Population::prepareGenerationChange() {

    /* randomly shuffle the training data */
    if (opts.shuffleTrainingData()) {
        rand64_t randstate = *rand;
        std::random_shuffle(this->inputs.begin(), this->inputs.end(), shuffleRand);
        *rand = randstate;
        std::random_shuffle(this->outputs.begin(), this->outputs.end(), shuffleRand);
    }

    /* inform species observers */
    for (auto i: this->speciesInspectors)
        i->bevor(this->species);

    /* calculate the fitness of all Organisms */
    if (this->fitnessCalculator != NULL) {
        if (opts.numThreads() > 1) {
            ParallelProcessor processor(opts.numThreads(), fitnessCalculator);

            for (auto &s : this->species)
                processor.add(s->getOrganisms());

            processor.start();
            processor.join();

            for (auto &s : this->species)
                s->update();
        } else {
            for (auto &s : this->species)
                s->calculateFittness(this->fitnessCalculator);
        }
    } else {
        for (auto &s : this->species)
            s->calculateFittness(this->inputs, this->outputs);
    }

    /* inform species observers */
    for (auto i: this->speciesInspectors)
        i->during(this->species);

//#define PAPER_FITNES_SHARING
// NOT WORKING!! fitness sharing wassnt actually used only described within the paper
#ifdef PAPER_FITNES_SHARING
    
    vector<shared_ptr<Organism>> organisms;
    vector<double> fitness;
    for (auto &s : this->species) {
        vector<shared_ptr<Organism>> currentOrganisms = s->getOrganisms();

        for (auto &o: currentOrganisms) {
            fitness.push_back(o->getFitness() / s->size());
            organisms.push_back(o);
        }
    }

    for (unsigned i = organisms.size() - 1; i > 0; i--) {
        for (unsigned n = 0; n < i; n++) {
            if (fitness[n] > fitness[n + 1]) {
                double tmpFitness                = fitness[n];
                shared_ptr<Organism> tmpOrganism = organisms[n];

                fitness[n]   = fitness[n + 1];
                organisms[n] = organisms[n + 1];

                fitness[n + 1]   = tmpFitness;
                organisms[n + 1] = tmpOrganism;
            }
        }
    }

    for (unsigned n = 0; n < organisms.size() * (1 - opts.survivalRate()); n++) 
        organisms[n]->kill();

    for (auto &s : this->species)
        s->removeDeaths();

    /* remove ectinct species */
    for (int i = 0; i < (int) this->species.size(); i++) {
        if (this->species[i]->size() == 0) {
            this->species.erase(this->species.begin() + i);
            i--;
        }
    }


#else

    /* kill the Organisms with the worst fitness */
    for (auto &s : this->species)
        s->killWorst();

#endif

    /* sort species by their best fitness */
    std::sort(this->species.begin(), this->species.end(), orderSpecies);

    /* inform species observers */
    for (auto i: this->speciesInspectors)
        i->after(this->species);

    /* check if we found a new best fitness */
    if (this->species[0]->getBestFitness() > this->bestFitness) {
        this->bestFitness = this->species[0]->getBestFitness();
        this->lastImproveGeneration = GenerationCounter::get();
    } 
        
    /* collect all survived Organisms for reproduction */
#ifdef PAPER_FITNES_SHARING
    organisms.clear();
#else
    vector<shared_ptr<Organism>> organisms;
#endif
    for (auto &s : this->species) {
        vector<shared_ptr<Organism>> currentOrganisms = s->getOrganisms();
        organisms.insert(organisms.end(), currentOrganisms.begin(), currentOrganisms.end());
    }

    /* sort the organisms by theyr fitness */
    std::sort(organisms.begin(), organisms.end(), orderOrganisms); 
    return organisms;
}

/* calculate the ofsprings per species */
vector<unsigned> Population::calculateOffspringsPerSpecies(unsigned nOffsprings) {

#define SOFTMAX_PROPABILITY
#ifdef SOFTMAX_PROPABILITY
    /* create a vector of propability distribution for each fittness */
    vector<double> propabilityDistribution;
    double expSum = 0;

    for (const auto &s : this->species) {
        propabilityDistribution.push_back(s->getAverageFitness());
        expSum += exp(propabilityDistribution.back());
    }

    for (unsigned i = 0; i < propabilityDistribution.size(); i++)
        propabilityDistribution[i] = exp(propabilityDistribution[i]) / expSum;

    for (unsigned i = 1; i < propabilityDistribution.size(); i++)
        propabilityDistribution[i] += propabilityDistribution[i - 1];

#else
    
    /* create a vector of propability distribution for each fittness */
    vector<double> propabilityDistribution;
    double overallAverageFitness = 0;

    for (const auto &s : this->species) {
        propabilityDistribution.push_back(s->getAverageFitness());
        overallAverageFitness += propabilityDistribution.back();
    }

    for (auto &p : propabilityDistribution)
        p /= overallAverageFitness;

    for (unsigned i = 1; i < propabilityDistribution.size(); i++)
        propabilityDistribution[i] += propabilityDistribution[i - 1];
#endif

    /* distribute the Organisms of the new generation 
     * based on the average fitness of each Species
     */
    vector<unsigned> offspringsPerSpecies(this->species.size(), 0);
    for (unsigned i = 0; i < nOffsprings; i++) {
        const double randValue = rand_range_d64(rand, 0.0, propabilityDistribution.back());
        
        for (unsigned p = 0; p < this->species.size(); p++) {
            if (propabilityDistribution[p] > randValue) {
                offspringsPerSpecies[p]++;
                break;
            }
        }
    }

    return offspringsPerSpecies;
}

/* calculate the interspecies offsprings */
vector<shared_ptr<Organism>> Population::createInterSpeciesOffsprings() {

    vector<shared_ptr<Organism>> offsprings;
    unsigned interSpeciesOffsprings = 0;
    if (this->species.size() > 1) {
        interSpeciesOffsprings = opts.populationSize() * opts.interSpeciesMatingPropability() + 
                                 rand_range_d64(rand, 0, 1);
    }

    for (unsigned i = 0; i < interSpeciesOffsprings; i++) {
        
        const unsigned index1 = rand64(rand) % this->species.size();
        unsigned index2       = rand64(rand) % this->species.size();

        while (index2 == index1) 
            index2 = rand64(rand) % this->species.size();
        
        offsprings.push_back(this->species[index1]->createOffspring(*this->species[index2]));
    }

    return offsprings;
}

/* clalculates the offsprings for each species 
 * returns the offsprings which arent similar to the species they came from
 */
vector<shared_ptr<Organism>> 
Population::createInSpeciesOffsprings(vector<unsigned> offspringsPerSpecies) {

    vector<shared_ptr<Organism>> offsprings;

    /* calculate the offsprings per species */
    for (unsigned i = 0; i < offspringsPerSpecies.size(); i++) {
        vector<shared_ptr<Organism>> currentOffsprings;
        currentOffsprings = this->species[i]->createOffsprings(offspringsPerSpecies[i]);

        /* try adding the offsprings to its species */
        for (auto &o : currentOffsprings) {
            if (this->species[i]->similarity(o) < opts.speciesSimilarityThreshold())
                this->species[i]->add(o);
            else
                offsprings.push_back(o);
        }
    }

    return offsprings;
}

/* sort the given offsprings into species (create new species if neccesarry) */
void Population::sortOffspringsIntoSpecies(vector<shared_ptr<Organism>> offsprings) {

    for (auto &o : offsprings) {

        bool newSpecies = true;
        for (auto &s : this->species) {
            if (s->similarity(o) < opts.speciesSimilarityThreshold()) {
                s->add(o);
                newSpecies = false;
                break;
            }
        }

        if (newSpecies) {
            this->species.push_back(make_shared<Species>(this->opts, this->geneHistory));
            this->species.back()->add(o); 
        }
    }
}

/* finish the generation chane by cleaning up the death individuals 
 * and increasing the GenerationCounter
 */
void Population::finishGenerationChange(vector<shared_ptr<Organism>> lastGeneration) {

    /* kill the remaining Organisms from the last generation */
    for (auto &o: lastGeneration)
        o->kill();

    for (auto &s : this->species)
        s->removeDeaths();

    /* remove ectinct species */
    for (int i = 0; i < (int) this->species.size(); i++) {
        if (this->species[i]->size() == 0) {
            this->species.erase(this->species.begin() + i);
            i--;
        }
    }

    /* increase the generation counter */
    GenerationCounter::onlyInstance.generation++;
}

/* increase the genaration counter and sort all genomes by theyr fitness */
void Population::calcNextGeneration() {

    /* calculate the Organisms for reproduction */
    vector<shared_ptr<Organism>> lastGeneration = this->prepareGenerationChange();

    /* calculate the interspecies offsprings 
     * (add one offsprings more based on the propability given from the decimal part )
     */
    vector<shared_ptr<Organism>> interSpeciesOffsprings = this->createInterSpeciesOffsprings();

    /* calculate the number of offsprings per species */
    vector<unsigned> offspringsPerSpecies = this->calculateOffspringsPerSpecies(
        opts.populationSize() - interSpeciesOffsprings.size()
    );

    /* calculate the inspecies offsprings */
    vector<shared_ptr<Organism>> inSpeciesOffsprings = this->createInSpeciesOffsprings(
        offspringsPerSpecies
    );

    /* sort the offsprings into species */
    this->sortOffspringsIntoSpecies(interSpeciesOffsprings);
    this->sortOffspringsIntoSpecies(inSpeciesOffsprings);
    
    /* finish the generation change by doing cleanups */
    this->finishGenerationChange(lastGeneration);
}

/* returns the best Organism */
shared_ptr<Organism> Population::getBestOrganism() {
    
    vector<shared_ptr<Organism>> organisms;

    if (this->fitnessCalculator != NULL) {
        if (opts.numThreads() > 1) {
            ParallelProcessor processor(opts.numThreads(), fitnessCalculator);

            for (auto &s : this->species)
                processor.add(s->getOrganisms());

            processor.start();
            processor.join();

            for (auto &s : this->species)
                s->update();
        } else {
            for (auto &s : this->species)
                s->calculateFittness(this->fitnessCalculator);
        }
    } else {
        for (auto &s : this->species)
            s->calculateFittness(this->inputs, this->outputs);
    }

    for (auto &s : this->species) {
        vector<shared_ptr<Organism>> currentOrganisms = s->getOrganisms();
        organisms.insert(organisms.end(), currentOrganisms.begin(), currentOrganisms.end());
    }

    double bestFitness = 0;
    shared_ptr<Organism> bestOrganism;

    for (const auto &o : organisms) {
        if (o->getFitness() > bestFitness) {
            bestFitness = o->getFitness();
            bestOrganism = o;
        }
    }

    return bestOrganism;
}

/* create all initial connections */
void Population::createInitialConnections(bool connectBias) {

    vector<shared_ptr<Organism>> organisms;

    for (auto &s : this->species) {
        vector<shared_ptr<Organism>> currentOrganisms = s->getOrganisms();
        organisms.insert(organisms.end(), currentOrganisms.begin(), currentOrganisms.end());
    }

    for (auto &o : organisms)
        o->createInitialConnections(connectBias);

    for (auto &s : this->species)
        s->reset();

    this->species.clear();
    this->sortOffspringsIntoSpecies(organisms);
    assert(this->species.size() >= 1);
}
