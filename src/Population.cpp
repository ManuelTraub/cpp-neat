#include "Population.h"
#include "Organism.h"
#include "Opts.h"
#include <memory>
#include <algorithm>

using std::shared_ptr;
using std::make_shared;
using namespace NEAT;

/* global random value */
rand64_t *Population::rand = new_rand64_t(time(NULL));

/* create a new Population */
Population::Population(vector<vector<double>> inputs, vector<vector<double>> outputs) {
    this->inputs  = inputs;
    this->outputs = outputs;

    this->species.push_back(make_shared<Species>());

    for (unsigned i = 0; i < Opts::populationSize(); i++)
        this->species.back()->add(make_shared<Organism>(inputs[0].size(), outputs[0].size()));

    this->species.back()->calculateOldesGenome();
}

/* sorts the Organisms and species as removing the deaths 
 * returns the organisms left for reproduction
 */
vector<shared_ptr<Organism>> Population::prepareGenerationChange() {

    vector<shared_ptr<Organism>> organisms;

    /* calculate the fitness of all Organisms */
    for (auto &s : this->species) {
        s->calculateFittness(inputs, outputs);
        vector<shared_ptr<Organism>> currentOrganisms = s->getOrganisms();
        organisms.insert(organisms.end(), currentOrganisms.begin(), currentOrganisms.end());
    }

    /* stort the organisms by theyr fitness */
    std::sort(organisms.begin(), organisms.end(), OrganismComparator());

    /* kill the Organisms with the worst fitness */
    unsigned deaths    = organisms.size() * Opts::survivalRate();
    unsigned killStart = organisms.size() - deaths;
    for (unsigned i = killStart; i < organisms.size(); i++)
        organisms[i]->kill();

    organisms.erase(organisms.begin() + killStart, organisms.end());

    /* remove all death Organisms from their Species */
    for (auto &s : this->species)
        s->removeDeaths();

    /* remove ectinct species */
    for (int i = 0; i < (int) this->species.size(); i++) {
        if (this->species[i]->size() == 0) {
            this->species.erase(this->species.begin() + i);
            i--;
        }
    }

    /* sort species by their best fitness */
    std::sort(this->species.begin(), this->species.end(), SpeciesComparator());
    
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
        
        for (unsigned p = 0; p < this->species.size(); i++) {
            if (propabilityDistribution[p] > randValue) {
                offspringsPerSpecies[p]++;
            }
        }
    }
        
    return offspringsPerSpecies;
}

/* calculate the interspecies offsprings */
vector<shared_ptr<Organism>> Population::calculateInterSpeciesOffsprings() {

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
        
        offsprings.push_back(this->species[index1]->createOffspring(*this->species[index2].get()));
    }

    return offsprings;
}

/* increase the genaration counter and sort all genomes by theyr fitness */
void Population::calcNextGeneration() {

    /* calculate the Organisms for reproduction */
    vector<shared_ptr<Organism>> organisms = this->prepareGenerationChange();

    /* calculate the interspecies offsprings 
     * (add one offsprings more based on the propability given from the decimal part )
     */
    vector<shared_ptr<Organism>> offsprings = this->calculateInterSpeciesOffsprings();

    /* calculate the number of offsprings per species */
    vector<unsigned> offspringsPerSpecies = this->calculateOffspringsPerSpecies(
        Opts::populationSize() - offsprings.size()
    );


    /* calculate the offsprings per species */
    for (unsigned i = 0; i < offspringsPerSpecies.size(); i++) {
        vector<shared_ptr<Organism>> currentOffsprings;
        currentOffsprings = this->species[i]->createOffsprings(offspringsPerSpecies[i]);

        /* try adding the offsprings to ist species */
        for (auto &o : currentOffsprings) {
            if (this->species[i]->similarity(o) < Opts::speciesSimilarityThreshold())
                this->species[i]->add(o);
            else
                offsprings.push_back(o);
        }
    }

    /* sort the remaining offsprings into species */
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
            this->species.back()->calculateOldesGenome();
        }
    }
    
    /* kill the remaining Organisms from the last generation */
    for (auto &o: organisms)
        o->kill();

    for (auto &s : this->species) {
        s->removeDeaths();
        s->calculateOldesGenome();
    }

    /* increase the generation counter */
    GenerationCounter::onlyInstance.generation++;
}
