#include "Organism.h"
#include "Species.h"
#include "Opts.h"

using namespace NEAT;

/* calculates the fitness of this */
void Organism::calculateFittness(vector<vector<double>> inputs, 
                                 vector<vector<double>> outputs,
                                 Species &species) {

    /* calculate the raw fitness of this */
    if (std::isnan(fitness)) {
        Phenotype phenotype(genome);
        this->fitness = phenotype.calculateError(inputs, outputs);
    }

    /* adjust fitness according to the size of the Species */
    this->fitness /= species.size();

    /* adjust fitness according to the age of the species 
     * younger species will get a fitness boost
     */
    if (species.age() < Opts::youthThreshold())
        this->fitness *= Opts::youthBoost();

    /* adjust fitness of stagnant species */
    if (species.stagnantAge() > Opts::stagnantThreshold())
        this->fitness *= Opts::stagnantPenalty();
}
