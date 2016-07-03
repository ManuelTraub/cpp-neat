#include "Organism.h"
#include "Species.h"

using namespace NEAT;

/* calculates the fitness of this */
void Organism::calculateFittness(vector<vector<double>> inputs, 
                                 vector<vector<double>> outputs,
                                 Species &species) {

    /* calculate the raw fitness of this */
    if (std::isnan(fitness)) {
        const Phenotype phenotype(genome);
        this->fitness = phenotype.calculateError(inputs, outputs);
    }

    /* adjust fittnes according to the size of the Species */
    this->fittnes /= species.size();

    /* adjust fitness according to the age of the species 
     * younger species will get a fitness boost
     */
    if (species.age() < Species::youthThreshold)
        this->fitness *= Species::youthBoost;

    /* adjust fitness of stagnant species */
    if (species.stagnantAge() > Species::stagnantThreshold)
        this->fitness *= species::stagnantPenalty;
}
