#include "Opts.h"

using namespace NEAT;

/* the only instance of the Opts */
Opts Opts::onlyInstance;

/* access the only instance of this */
Opts &Opts::getInstance() {
    return onlyInstance;
}

/* initialize the options of this */
Opts::Opts() {

    /* threshold for young species to get a fitness boost */
    _youthThreshold = 10;

    /* the fitness boost a young generation gets (in percent / 100) */
    _youthBoost = 0.99;

    /* threshold after which a stagnant species will get a fittnes penalty */
    _stagnantThreshold = 20;

    /* the penalty for a stagnant species (in percent / 100) */
    _stagnantPenalty = 100;

    /* propability for a mutation only offspring */
    _mutationOnlyPropability = 0.25;

    /* threshold for species size to have the fittest Organism survive an epoch */
    _fittestSurvivalThreshold = 5;

    /* the similarity threshold for Organism to be in the same species */
    _similarityThreshold = 3.0;

}
