/**
 * global NEAT options
 */
#ifndef __OPTS_H__
#define __OPTS_H__

#include "Gene.h"
#include <vector>
#include <unordered_map>

using std::vector;
using std::unordered_map;
using std::make_pair;

#ifdef __DEBUG__ 

/* adds an option to the Opts class */
#define addOption(type, name, value) \
    public  : type _##name = value;  \
    public  : static type name() { return onlyInstance._##name; }

#else

/* adds an option to the Opts class */
#define addOption(type, name, value) \
    private : type _##name = value;  \
    public  : static type name() { return onlyInstance._##name; }

#endif /* __DEBUG__ */

namespace NEAT {
    
    class Opts {

        private:

            /* the only instance of the Opts */
            static Opts onlyInstance;

            /* private constructor and destructor to make this a singleton */
            Opts() { }

            /* private destructor to avoid deletion of the singleton */
            ~Opts() { }

            /* delete copy constructor to disalow copies of this */
            Opts(const Opts &) = delete;

            /* threshold for young species to get a fitness boost */
            addOption(unsigned, youthThreshold, 10)

            /* the fitness boost a young generation gets (in percent / 100) */
            addOption(double, youthBoost, 1.01)

            /* threshold after which a stagnant species will get a fittnes penalty */
            addOption(unsigned, stagnantThreshold, 15)

            /* threshold after which all species will get a fittnes penalty 
             * if the generation is stagnant
             */
            addOption(unsigned, generationStagnantThreshold, 20)

            /* the penalty for a stagnant species (in percent / 100) */
            addOption(double, stagnantPenalty, 0.01)

            /* propability for a mutation only offspring */
            addOption(double, mutationOnlyPropability, 0.25)

            /* threshold for species size to have the fittest Organism survive an epoch */
            addOption(unsigned, fittestSurvivalThreshold, 5)

            /* the similarity threshold for Organism to be in the same species */
            addOption(double, speciesSimilarityThreshold, 3)

            /* the property for disabeling a connection gene if 
             * eighter one parrent has the gene disabled
             */
            addOption(double, recombinationGeneDisablePropability, 0.75)

            /* the survival rate of the population during generation change */
            addOption(double, survivalRate, 0.2)

            /* propability of interspecies mating */
            addOption(double, interSpeciesMatingPropability, 0.001)

            /* desired number of Organism per generation */
            addOption(unsigned, populationSize, 150)

            /* propability of adding a new Neuron during mutation */
            addOption(double, addNeuronMutationPropability, 0.03)

            /* propability of adding a new Connection between two Neurons during mutation */
            addOption(double, addConnectionMutationPropability, 0.05)

            /* the propability of mutating the weights of a Genome during mutation */
            addOption(double, mutateWeightsPropability, 0.9)

            /* the gene multiplayer for calculating similarity */
            addOption(double, geneMultiplier, 1.0)

            /* avg weight diference multiplier when calculating the similarity */
            addOption(double, weightMultiplier, 0.4)

            /* the perturbe weight range start and end */
            addOption(double, perturbeWeightRangeStart, -2.5)
            addOption(double, perturbeWeightRangeEnd, 2.5)

            /* the propability of setting a connection weight to 
             * a new random value during mutation 
             */
            addOption(double, hardMutationPropability, 0.1)

            /* threshold for boosting structural mutation in the first generations */
            addOption(unsigned, structuralMutationThreshold, 0);

            /* the boost for structural mutation in the first generations */
            addOption(double, structuralMutationBoost, 1);

            /* propability to do no mutation during recombination */
            addOption(double, mateOnlyPropability, 0.2);

            /* propability of disabeling an inerhited enabled connection 
             * (the propability of disableing a connection if eighter one parrent 
             *  is disabled is 1 - 0.5 * connectionDisablePropability)
             */
            addOption(double, connectionDisablePropability, 0.5);

            /* propability of using average mating */
            addOption(double, avgMatingPropability, 0.4);

            /* inicates that recurent connections are enabled */
            addOption(bool, allowRecurentConnections, false);

            /* every x generation penalize the worst performing Species */
            addOption(unsigned, penealizeEveryXGeneration, 30);

            /* the age after which a Species can be penalized at 
             * an penealizeEveryXGeneration event 
             */
            addOption(unsigned, penalizeAge, 20);

#ifdef __DEBUG__
            static Opts &getInstance() { return onlyInstance; }
#endif 

    };

}

#undef addOption

#endif /* __OPTS_H__ */
