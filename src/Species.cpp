#include "Species.h"
#include "Organism.h"
#include "Opts.h"
#include <algorithm>

using namespace NEAT;
using std::make_shared;

/* global random value */
rand64_t *Species::rand = new_rand64_t(time(NULL));

/* calculate the simmilarity of the given Genotype
 * to the oldest genome of this species 
 */
double Species::similarity(shared_ptr<Organism> &o) const { 
    return oldestGenome.similarity(o->getGenome()); 
}

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

    oldestGenome = Genotype();

    /* recalculate the oldest Organism */
    for (const auto &o : organisms)
        if (oldestGenome.genomeAge() < o->getGenome().genomeAge())
            oldestGenome = o->getGenome();
}


/* clalculate the fittness of all Organism */
void Species::calculateFittness(vector<vector<double>> inputs, vector<vector<double>> outputs) {
    
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
// TODO add structural mutations
shared_ptr<Organism> Species::createOffspring(unsigned index) {
    
    if (rand_range_d64(rand, 0, 1) < Opts::mutationOnlyPropability() || this->size() == 1) {

        /* create new Organism and mutate its Genome */
        shared_ptr<Organism> offspring = make_shared<Organism>(organisms[index]->getGenome());
        //offspring->getGenome().mutate(); TODO
        return offspring;
        
    } else {
        
        /* find a random organisms for reproduction */
        unsigned index2 = rand64(rand) % this->size();
        while (index2 == index)
            index2 = rand64(rand) % this->size();

        return make_shared<Organism>(Genotype(organisms[index]->getGenome(), 
                                              organisms[index2]->getGenome()));
    }
}

/* calculates the given number of offsprings for the next generation */
vector<shared_ptr<Organism>> Species::createOffsprings(unsigned nOffsprings) {
    
    /* let the fittest Organism survive if this species is big enought */
    vector<shared_ptr<Organism>> newGeneration;
    if (this->size() > Opts::fittestSurvivalThreshold() && nOffsprings > 1) {

        /* copy the fittest Organism */
        newGeneration.push_back(make_shared<Organism>(this->organisms[0]->getGenome()));
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

    return make_shared<Organism>(Genotype(this->organisms[thisIndex]->getGenome(), 
                                          other.organisms[otherIndex]->getGenome()));
    
}
