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

/**
 * Adds an option to the Opts class.
 * It implements getter setter, and resetter methods
 * */
#define addOption(type, name, value)                    \
    private :                                           \
                                                        \
        /* the actual value */                          \
        type _##name = value;                           \
                                                        \
    public  :                                           \
                                                        \
        /* getter method */                             \
        type name() const { return _##name; }           \
                                                        \
        /* setter method when opts aren't frozen */     \
        void name(type arg) {                           \
            assert(!this->frozen);                      \
            if (!this->frozen)                          \
                _##name = arg;                          \
        }                                              
                                                      

namespace NEAT {

        
        class Opts {

            private:

                /* indicates if this can be edited */
                bool frozen = false;

                /* delete copy constructor to disalow copies of this */
                Opts(const Opts &) = delete;

            public:

                /* standart constructor */
                Opts() { }
                
                /* returns whether this is frozen */
                bool isFrozen() const { return frozen; }

                /* freezes this so its values can not longer be changed */
                Opts &freeze() { frozen = true; return *this; }

            /* number of input neurons (if 0 set by using the training data) */
            addOption(unsigned, numInputNeurons, 0)

            /* number of output neurons (if 0 set by using the training data) */
            addOption(unsigned, numOutputNeurons, 0)
                
            /* propability for a mutation only offspring */
            addOption(double, mutationOnlyPropability, 0.25)

            /* threshold for species size to have the fittest Organism survive an epoch */
            addOption(unsigned, fittestSurvivalThreshold, 5)

            /* the similarity threshold for Organism to be in the same species */
            addOption(double, speciesSimilarityThreshold, 3.2)

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

            /* the propability of mutating the neurons of a Genome during mutation */
            addOption(double, mutateNeuronsPropability, 0.5)

            /* indicates whether the gradient shoud be recombinated or choosen from the fitter */
            addOption(bool, recombinateNeurons, true);

            /* the gene multiplayer for calculating similarity */
            addOption(double, geneMultiplier, 1.0)

            /* avg weight difference multiplier when calculating the similarity */
            addOption(double, weightMultiplier, 0.4)

            /* avg gradient difference multiplier when calculating the similarity */
            addOption(double, gradientMultiplier, 0.1)

            /* the perturbe weight range start and end */
            addOption(double, perturbeWeightRangeStart, -2.5)
            addOption(double, perturbeWeightRangeEnd, 2.5)

            /* the weight range for seting weights during mutation */
            addOption(double, mutationSetWeightRangeStart, -2.5)
            addOption(double, mutationSetWeightRangeEnd, 2.5)

            /* the weight range for creating a ConnectionGene */
            addOption(double, creationWeightRangeStart, -1) 
            addOption(double, creationWeightRangeEnd, 1)

            /* the max weight range for pertubing weight during mutation */
            addOption(double, mutationMaxPertubeWeightRangeStart, -4096) 
            addOption(double, mutationMaxPertubeWeightRangeEnd, 4096)

            /* the propability of setting a connection weight to 
             * a new random value during mutation 
             */
            addOption(double, hardMutationPropabilityWeights, 0.1)

            /* propability to do no mutation during recombination */
            addOption(double, mateOnlyPropability, 0.2);

            /* propability of using average mating */
            addOption(double, avgMatingPropability, 0.4);

            /* inicates that recurent connections are enabled */
            addOption(bool, allowRecurentConnections, false);

            /* propability to reenable a gene during mating */
            addOption(double, geneReenablePropability, 0.25);

            /* indicates that the training data should be shuffled after each generation */
            addOption(bool, shuffleTrainingData, true);

            /* the net activation function used in the neurons */
            addOption(ActivationFunction, netActivationFunction, ActivationFunction::SIGMOID);

            /* the activation function for output neurons (LENGTH => use netActivationFunction) */
            addOption(ActivationFunction, outputActivationFunction, ActivationFunction::LENGTH);

            /* indicates that a neuronal network
             * should be reset bevor each run */
            addOption(bool, resetNetBeforRun, false);

            /* the offset to the agin propability for mutating a certain connection
             * Propability = 1/(age + 1) + offset
             */
            addOption(double, ageingPropabilityOffset, 1.0);

            /* indicate that we are alowing insertion of neurons into BIAS neurons */
            addOption(bool, allowNeuronIntoBiasConnectionInsert, false);
            
            /* indicates that the network should be updated layer by layer */
            addOption(bool, layeredNetwork, true);

            /* indicates that the fitness should always be recalculated for each organism */
            addOption(bool, alwaysRecalculateFitness, false);

            /* number of threads */
            addOption(unsigned, numThreads, 1);

        };
}

#undef addOption

#endif /* __OPTS_H__ */
