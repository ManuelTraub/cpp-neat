#include "Species.h"

using namespace NEAT;
using std::make_shared;



/* threshold for young species to get a fitness boost */
unsigned Species::youthThreshold = 10;

/* the fitness boost a young generation gets (in percent / 100) */
double Species::youthBoost = 0.99;

/* threshold after which a stagnant species will get a fittnes penalty */
unsigned Species::stagnantThreshold = 20;

/* the penalty for a stagnant species (in percent / 100) */
double Species::stagnantPenalty = 100;

/* propability for a mutation only offspring */
double Species::mutationOnlyPropability = 0.25;

/* threshold for species size to have the fittest Organism survive an epoch */
unsigned Species::fittestSurvivalThreshold = 5;

/* the similarity threshold for Organism to be in the same species */
double Species::similarityThreshold = 3.0;

/* global random value */
rand64_t *Species::rand = new_rand64_t(time(NULL));

/* recalculates the oldes genome */
void Species::refresh() {

    /* remove all deaths */
    for (int i = 0; i < (int) organisms.size(); i++) {
        if (!organisms[i]->isAlive()) {
            organisms.erase(organisms.begin() + i);
            i--;
        }
    }

    /* recalculate the oldest Organism */
    for (const auto &o : organisms)
        if (oldes == nullptr || oldest->genomeAge() < o->genomeAge())
            oldest = o;

    /* if this species died remove the oldes */
    if (this->size() == 0)
        oldest = nullptr;
}

/* clalculate the fittness of all Organism */
void Species::calculateFittness(vector<vector<double>> inputs, vector<vector<double>> outputs) {
    
    this->refresh();
    
    for (auto &o : organisms) {
        o->calculateFittness(inputs, outputs, *this);

        /* actualitze the best fitness of this */
        if (o->getFitness() < this->bestFittness) {
            this->bestFittness = o->getFitness();
            this->lastImproveGeneration = GenerationCounter::get();
        }
    }

    /* sort the organisms by fitness */
    std::sort(organisms.begin(), organisms.end());
}

/* chose a Organisms for reproduction, propability based on the organisms fittnes */
vector<unsigned> Species::chooseOrganisms(unsigned nOffsprings) {
    
    /* create vector for the choosen indices */
    vector<unsigned> choosen(nOffsprings);

    /* create a propability vector for choosing the individum for reproduction */
    vector<double> propability(organisms.size());

    double maxPropability = 0.0;
    for (unsigned i = 0; i < organisms.size(); i++) {
        propability[i] = organisms[i]->getFitness() / organisms[0]->getFitness();
        maxPropability += propability[i];
    }

    for (unsigned i = 0; i < nOffsprings; i++) {
        const double randValue = rand_range_d64(rand, 0.0, maxPropability);
        for (unsigned o = 0; o < organisms.size(); o++) {
            if (propability[o] < randValue) {
                choosen[i] = o;           
            }
        }
    }

    return choosen;
}

/* creates a new offspring from the Organism with the given index */
shared_ptr<Organism> Species::createOffspring(unsigned index) {
    
    if (rand_range_d64(rand, 0, 1) < mutationOnlyPropability || this->size() == 1) {

        /* create new Organism and mutate its Genome */
        shared_ptr<Organism> offspring = make_shared<Organism>(organisms[index]->getGenome());
        offspring->getGenome()->mutate();
        return offspring;
        
    } else {
        
        /* find a random organisms for reproduction */
        unsigned index2 = rand64(rand) % this->size();
        while (index2 == index)
            index2 = rand64(rand) % this->size();

        return make_shared<Organism>(Genome(organisms[index]->getGenome(), 
                                            organisms[index2]->getGenome()));
    }
}

/* calculates the given number of offsprings for the next generation */
vector<shared_ptr<Organism>> Species::createOffsprings(unsigned nOffsprings) {
    
    this->refresh();

    /* let the fittest Organism survive if this species is big enought */
    vector<shared_ptr<Organism>> newGeneration;
    if (this->size() > fittestSurvivalThreshold && nOffsprings > 1) {

        /* copy the fittest Organism */
        newGeneration.push_back(make_shared<Organism>(this->organisms[0]->getGenome()));
        nOffsprings--;
    }

    /* choose Organisms for reproduction */
    vector<unsigned> choosen = this->choseOrganisms(nOffsprings);

    /* create the new generation */
    for (const auto &i : choosen)
        newGeneration.push_back(this->createOffspring(i));

    return newGeneration;
}

/* calculate an inter species offspring */
shared_ptr<Organism> Species::createOffspring(Species &other) {
    
    this->refresh();
    other->refresh();

    /* choose Organisms for reproduction */
    const thisIndex  = this->chooseOrganisms(1)[0];
    const otherIndex = other->chooseOrganisms(1)[0];

    return make_shared<Organism>(Genome(this->organisms[thisIndex]->getGenome(), 
                                        other->organisms[otherIndex]->getGenome()));
    
}
