#include "Population.h"
#include "Organism.h"
#include "Opts.h"
#include <memory>
#include <algorithm>

using std::shared_ptr;
using std::make_shared;
using namespace NEAT;

/* global random value */
rand64_t *Population::rand = new_rand64_t();

/* create a new Population */
Population::Population(vector<vector<double>> inputs, vector<vector<double>> outputs) {
    this->inputs  = inputs;
    this->outputs = outputs;
    this->bestFitness = 0;
    this->lastImproveGeneration = GenerationCounter::get();

    this->species.push_back(make_shared<Species>());

    for (unsigned i = 0; i < Opts::populationSize(); i++)
        this->species.back()->add(make_shared<Organism>(inputs[0].size(), outputs[0].size()));
}

/* sorts the Organisms and species as removing the deaths 
 * returns the organisms left for reproduction
 */
vector<shared_ptr<Organism>> Population::prepareGenerationChange() {

    /* randomly shuffle the training data */
    rand64_t randstate = *rand;
    std::random_shuffle(this->inputs.begin(), this->inputs.end(), shuffleRand);
    *rand = randstate;
    std::random_shuffle(this->outputs.begin(), this->outputs.end(), shuffleRand);

    /* calculate the fitness of all Organisms */
    for (auto &s : this->species)
        s->calculateFittness(this->inputs, this->outputs);

    /* kill the Organisms with the worst fitness */
    for (auto &s : this->species)
        s->killWorst();

    /* sort species by their best fitness */
    std::sort(this->species.begin(), this->species.end(), SpeciesComparator());

    /* every x generations penalize the worst species over n years 
     * (best species will never be penalized) 
     */
    if (GenerationCounter::get() % Opts::penealizeEveryXGeneration()) {
        for (unsigned i = this->species.size() - 1; i > 0; i--) {
            if (this->species[i]->age() > Opts::penalizeAge()) {
                this->species[i]->penalize();
                break;
            }
        }
    }
    
    /* check if we found a new best fitness */
    if (this->species[0]->getBestFitness() > this->bestFitness) {
        this->bestFitness = this->species[0]->getBestFitness();
        this->lastImproveGeneration = GenerationCounter::get();
    } 
        
    /* penalize all species but the best two if 
     * we havent improved within the last n generations 
     */
    if (this->lastImproveGeneration > Opts::generationStagnantThreshold())
        for (unsigned i = 2; i < this->species.size(); i++)
            this->species[i]->penalize();

    /* collect all survived Organisms for reproduction */
    vector<shared_ptr<Organism>> organisms;
    for (auto &s : this->species) {
        vector<shared_ptr<Organism>> currentOrganisms = s->getOrganisms();
        organisms.insert(organisms.end(), currentOrganisms.begin(), currentOrganisms.end());
    }

    /* sort the organisms by theyr fitness */
    std::sort(organisms.begin(), organisms.end(), OrganismComparator());
    return organisms;
}

/* calculate the ofsprings per species */
vector<unsigned> Population::calculateOffspringsPerSpecies(unsigned nOffsprings) {
    
    /* create a vector of propability distribution for each fittness */
    vector<double> propabilityDistribution;
    double overallAverageFitness = 0;

    for (const auto &s : this->species) {
        propabilityDistribution.push_back(s->getAverageAdjustedFitness());
        overallAverageFitness += propabilityDistribution.back();
    }

    overallAverageFitness /= this->species.size();

    double propabilitySum = 0;
    for (auto &p : propabilityDistribution) {
        p /= overallAverageFitness;
        propabilitySum += p;
    }

    for (unsigned i = 1; i < propabilityDistribution.size(); i++)
        propabilityDistribution[i] += propabilityDistribution[i - 1];

    /* distribute the Organisms of the new generation 
     * based on the average fitness of each Species
     */
    vector<unsigned> offspringsPerSpecies(this->species.size(), 0);
    for (unsigned i = 0; i < nOffsprings; i++) {
        const double randValue = rand_range_d64(rand, 0.0, propabilitySum);
        
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
        interSpeciesOffsprings = Opts::populationSize() * Opts::interSpeciesMatingPropability() + 
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
            if (this->species[i]->similarity(o) < Opts::speciesSimilarityThreshold())
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
            if (s->similarity(o) < Opts::speciesSimilarityThreshold()) {
                s->add(o);
                newSpecies = false;
                break;
            }
        }

        if (newSpecies) {
            this->species.push_back(make_shared<Species>());
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
/*
    printf("\t");
    for (const auto &s : this->species)
        printf("%u ", s->size());
    printf("\n");
*/
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
        Opts::populationSize() - interSpeciesOffsprings.size()
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

    for (auto &s : this->species) {
        s->calculateFittness(inputs, outputs);
        vector<shared_ptr<Organism>> currentOrganisms = s->getOrganisms();
        organisms.insert(organisms.end(), currentOrganisms.begin(), currentOrganisms.end());
    }

    double bestFitness = 0;
    shared_ptr<Organism> bestOrganism;

    for (const auto &o : organisms) {
        if (o->getOriginalFitness() > bestFitness) {
            bestFitness = o->getOriginalFitness();
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
    assert(this->species.size() == 1);
}
