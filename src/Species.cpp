#include "Species.h"
#include "Organism.h"
#include <algorithm>

using namespace NEAT;
using std::make_shared;

/* global random value */
rand64_t *Species::rand = new_rand64_t();

/* remove all dead Organisms */
void Species::removeDeaths() {

    for (int i = 0; i < (int) organisms.size(); i++) {
        if (!organisms[i]->isAlive()) {
            organisms.erase(organisms.begin() + i);
            i--;
        }
    }
}

/* clalculate the fittness of all Organism */
void Species::calculateFittness(vector<vector<double>> inputs, vector<vector<double>> outputs) {

    this->averageFitness = 0;
    
    for (auto &o : this->organisms) {
        o->calculateFittness(inputs, outputs);

        /* actualitze the best fitness of this */
        if (std::isnan(this->bestFitness) ||
            o->getFitness() > this->bestFitness) {

            this->bestFitness           = o->getFitness();
            this->lastImproveGeneration = GenerationCounter::get();
        }

        /* actualitze the average fitness of this */
        this->averageFitness += o->getFitness();
    }

    this->averageFitness /= this->organisms.size();

    /* sort the organisms by theyr adjusted fitness */
    std::sort(organisms.begin(), organisms.end(), orderOrganisms);
}

/* creates a new offspring from the Organism with the given index */
shared_ptr<Organism> Species::createOffspring(unsigned index) {
    
    if (rand_range_d64(rand, 0, 1) < opts.mutationOnlyPropability() || this->size() == 1) {

        /* create new Organism and mutate its Genome */
       shared_ptr<Organism> offspring = make_shared<Organism>(this->organisms[index]->clone());
       offspring->mutate();
       return offspring;
        
    } else {
        
        /* find a random organisms for reproduction */
        unsigned index2 = rand64(rand) % this->size();
        while (index2 == index)
            index2 = rand64(rand) % this->size();

        shared_ptr<Organism> offspring = make_shared<Organism>(
            *this->organisms[index], 
            *this->organisms[index2],
            opts
        );

        if (rand_range_d64(rand, 0, 1) > opts.mateOnlyPropability())
            offspring->mutate();

        return offspring;
    }
}

/* calculates the given number of offsprings for the next generation */
vector<shared_ptr<Organism>> Species::createOffsprings(unsigned nOffsprings) {
    
    /* let the fittest Organism survive if this species is big enought */
    vector<shared_ptr<Organism>> newGeneration;
    if (nOffsprings > opts.fittestSurvivalThreshold()) {

        /* copy the fittest Organism */
        newGeneration.push_back(make_shared<Organism>(*this->organisms[0]));
        nOffsprings--;
    }

    /* create the new generation */
    for (unsigned i = 0; i < nOffsprings; i++) {
        const unsigned index = rand64(rand) % this->organisms.size();
        newGeneration.push_back(this->createOffspring(index));
    }

    return newGeneration;
}

/* calculate an inter species offspring */
shared_ptr<Organism> Species::createOffspring(Species &other) {

    /* choose Organisms for reproduction */
    const unsigned thisIndex  = rand64(rand) % this->organisms.size();
    const unsigned otherIndex = rand64(rand) % other.organisms.size();

    return make_shared<Organism>(
        *this->organisms[thisIndex], 
        *other.organisms[otherIndex],
        opts
    );
}

/* kills the worst performing Organisms 
 * (keeps at least one Organism) 
 */
void Species::killWorst() {
    
    unsigned killStart = this->organisms.size() * opts.survivalRate() + 1;

    for (unsigned i = killStart; i < this->organisms.size(); i++)
        organisms[i]->kill();

    organisms.erase(organisms.begin() + killStart, organisms.end());

    /* recalculate the average fitness */
    this->averageFitness = 0;
    
    for (auto &o : this->organisms)
        this->averageFitness += o->getFitness();

    this->averageFitness /= this->organisms.size();
}

namespace NEAT {

    /* compares two Species based on theyr best fittness */
    bool orderSpecies(const shared_ptr<Species> &a, const shared_ptr<Species> &b) {
        return a->getBestFitness() > b->getBestFitness();
    }
}
