/**
 * global NEAT options
 */
#ifndef __OPTS_H__
#define __OPTS_H__

#include "Gene.h"
#include "QuadTree.h"
#include <vector>
#include <unordered_map>

using std::vector;
using std::unordered_map;
using std::make_pair;

namespace NEAT {
    
    class Opts {

        private:

            /* the only instance of the Opts */
            static Opts onlyInstance;

            /* private constructor and destructor to make this a singleton */
            Opts();

            /* private destructor to avoid deletion of the singleton */
            ~Opts() { }

            /* delete copy constructor to disalow copies of this */
            Opts(const Opts &) = delete;

            /* threshold for young species to get a fitness boost */
            unsigned _youthThreshold;

            /* the fitness boost a young generation gets (in percent / 100) */
            double _youthBoost;

            /* threshold after which a stagnant species will get a fittnes penalty */
            unsigned _stagnantThreshold;

            /* the penalty for a stagnant species (in percent / 100) */
            double _stagnantPenalty;

            /* propability for a mutation only offspring */
            double _mutationOnlyPropability;

            /* threshold for species size to have the fittest Organism survive an epoch */
            unsigned _fittestSurvivalThreshold;

            /* the similarity threshold for Organism to be in the same species */
            double _similarityThreshold;


        public:

            /* access the only instance of this */
            static Opts &getInstance();

            /* threshold for young species to get a fitness boost */
            static unsigned youthThreshold() { return onlyInstance._youthThreshold; }

            /* the fitness boost a young generation gets (in percent / 100) */
            static double youthBoost() { return onlyInstance._youthBoost; }

            /* threshold after which a stagnant species will get a fittnes penalty */
            static unsigned stagnantThreshold() { return onlyInstance._stagnantThreshold; }

            /* the penalty for a stagnant species (in percent / 100) */
            static double stagnantPenalty() { return onlyInstance._stagnantPenalty; }

            /* propability for a mutation only offspring */
            static double mutationOnlyPropability() { return onlyInstance._mutationOnlyPropability; }

            /* threshold for species size to have the fittest Organism survive an epoch */
            static unsigned fittestSurvivalThreshold() { return onlyInstance._fittestSurvivalThreshold; }

            /* the similarity threshold for Organism to be in the same species */
            static double similarityThreshold() { return onlyInstance._similarityThreshold; }

    };

}

#endif /* __OPTS_H__ */
