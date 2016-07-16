#include "Species.h"
#include "Organism.h"
#include "Opts.h"
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


/* calculates the new oldest genome */
void Species::calculateOldesGenome() {

    oldestGenome = nullptr;

    /* recalculate the oldest Organism */
    for (const auto &o : organisms) 
        if (oldestGenome == nullptr || oldestGenome->genomeAge() < o->getGenome().genomeAge()) 
            oldestGenome = make_shared<Genotype>(o->getGenome());
}


/* clalculate the fittness of all Organism */
void Species::calculateFittness(vector<vector<double>> inputs, vector<vector<double>> outputs) {

    this->averageAdjustedFitness = 0;
    
    for (auto &o : organisms) {
        o->calculateFittness(inputs, outputs, this->size(), this->age(), this->stagnantAge());

        /* actualitze the best fitness of this */
        if (o->getOriginalFitness() < this->bestOriginalFitness) {
            this->bestOriginalFitness   = o->getOriginalFitness();
            this->lastImproveGeneration = GenerationCounter::get();
        }

        /* actualitze the average adjusted fitness of this */
        this->averageAdjustedFitness += o->getAdjustedFitness();
    }

    this->averageAdjustedFitness /= this->size();

    /* sort the organisms by theyr adjusted fitness */
    std::sort(organisms.begin(), organisms.end(), OrganismComparator());
}

/* chose a Organisms for reproduction, propability based on the organisms fittnes */
vector<unsigned> Species::chooseOrganisms(unsigned nOffsprings) {
    
    /* create vector for the choosen indices */
    vector<unsigned> choosen(nOffsprings);

    /* create a propability vector for choosing the individum for reproduction */
    vector<double> propability(organisms.size());

    double maxPropability = 0.0;
    for (unsigned i = 0; i < organisms.size(); i++) {
        propability[i] = organisms[i]->getAdjustedFitness() / organisms[0]->getAdjustedFitness();
        maxPropability += propability[i]; 
    }

    for (unsigned i = 1; i < propability.size(); i++)
        propability[i] += propability[i - 1];

    for (unsigned i = 0; i < nOffsprings; i++) {
        const double randValue = rand_range_d64(rand, 0.0, maxPropability);
        for (unsigned o = 0; o < organisms.size(); o++) {
            if (propability[o] > randValue) {
                choosen[i] = o;           
                break;
            }
        }
    }

    return choosen;
}

/* creates a new offspring from the Organism with the given index */
shared_ptr<Organism> Species::createOffspring(unsigned index) {
    
    if (rand_range_d64(rand, 0, 1) < Opts::mutationOnlyPropability() || this->size() == 1) {

        /* create new Organism and mutate its Genome */
       shared_ptr<Organism> offspring = make_shared<Organism>(this->organisms[index]->clone());
       offspring->mutate();
       return offspring;
        
    } else {
        
        /* find a random organisms for reproduction */
        unsigned index2 = rand64(rand) % this->size();
        while (index2 == index)
            index2 = rand64(rand) % this->size();

        shared_ptr<Organism> offspring = make_shared<Organism>(*this->organisms[index], 
                                                               *this->organisms[index2]);

        if (rand_range_d64(rand, 0, 1) > Opts::mateOnlyPropability())
            offspring->mutate();

        return offspring;
    }
}

/* calculates the given number of offsprings for the next generation */
vector<shared_ptr<Organism>> Species::createOffsprings(unsigned nOffsprings) {
    
    /* let the fittest Organism survive if this species is big enought */
    vector<shared_ptr<Organism>> newGeneration;
    if (nOffsprings > Opts::fittestSurvivalThreshold()) {

        /* copy the fittest Organism */
        newGeneration.push_back(make_shared<Organism>(*this->organisms[0]));
        nOffsprings--;
    }

    /* choose Organisms for reproduction */
    vector<unsigned> choosen = this->chooseOrganisms(nOffsprings);

    /* create the new generation */
    for (const auto &i : choosen)
        newGeneration.push_back(this->createOffspring(i));

    return newGeneration;
}

/* calculate an inter species offspring */
shared_ptr<Organism> Species::createOffspring(Species &other) {

    /* choose Organisms for reproduction */
    const unsigned thisIndex  = this->chooseOrganisms(1)[0];
    const unsigned otherIndex = other.chooseOrganisms(1)[0];

    return make_shared<Organism>(*this->organisms[thisIndex], *other.organisms[otherIndex]);
}
