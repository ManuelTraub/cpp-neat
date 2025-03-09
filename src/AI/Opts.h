/**
 * global NEAVC options
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
                                                      

namespace NEAVC {

    namespace AI {
        
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

                /* the different spiking econdigns */
                enum class SpikeCoding { FREQUENCY, TIME, TIME_WINDOW };

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

            /* propability of removing a Neuron during mutation */
            addOption(double, removeNeuronMutationPropability, 0)

            /* propability of removing a new Connection between two Neurons during mutation */
            addOption(double, removeConnectionMutationPropability, 0)

            /* the propability of mutating the weights of a Genome during mutation */
            addOption(double, mutateWeightsPropability, 0.9)

            /* the propability of mutating the neurons of a Genome during mutation */
            addOption(double, mutateNeuronsPropability, 0.5)

            /* the propability of mutating the function of a genome during mutating */
            addOption(double, mutateFunctionPropability, 0);

            /* indicates whether the gradient shoud be recombinated or choosen from the fitter */
            addOption(bool, recombinateNeurons, true);

            /* the gene multiplayer for calculating similarity */
            addOption(double, geneMultiplier, 1.0)

            /* avg weight difference multiplier when calculating the similarity */
            addOption(double, weightMultiplier, 0.4)

            /* avg gradient difference multiplier when calculating the similarity */
            addOption(double, gradientMultiplier, 0.1)

            /* avg function difference multiplier when calculating the similarity */
            addOption(double, functionMultiplier, 1.0)

            /* the structure multiplayer for calculating similarity */
            addOption(double, structureMultiplier, 1.0);

            /* avg gradient difference multiplier when calculating the similarity */
            addOption(double, sizeMultiplier, 0.5)

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

            /* indicates wheter to randomly coose the activation function */
            addOption(bool, randomActivationFunction, false);

            /* the activation function for output neurons (LENGTH => use netActivationFunction) */
            addOption(ActivationFunction, outputActivationFunction, ActivationFunction::LENGTH);

            /* the number of simulation steps which are neccessary 
             * to compute on output in a spiking neural network */
            addOption(unsigned, spikingNumSimulationStepsPerOutoput, 8);

            /* the maxium nmber of simulation steps to wait for an spike to occour */
            addOption(unsigned, spikingMaxOutputSiumlationSteps, 128);

            /* the spike encoding */
            addOption(SpikeCoding, spikeOutputEncoding, SpikeCoding::FREQUENCY);

            /* indicates that a (spinking/reccurent) neuronal network 
             * should be reset bevor each run */
            addOption(bool, resetNetBeforRun, false);

            /* the propability of evolving a muscle structure gene */
            addOption(double, muscleTissuePropability, 0.1);

            /* the propability of evolving a sensor structure gene */
            addOption(double, sensorTissuePropability, 0.05);

            /* the propability of adding structure during mutation */
            addOption(double, addStructureMutationPropability, 0.01);

            /* the propability of removing structure during mutation */
            addOption(double, removeStructureMutationPropability, 0.005);

            /* the propability of mutating structure type during mutation */
            addOption(double, mutateStructureTypePropability, 0.02);

            /* the propability of mutating structure size during mutation */
            addOption(double, mutateStructureSizePropability, 0.9);

            /* the offset to the agin propability for mutating a certain connection
             * Propability = 1/(age + 1) + offset
             */
            addOption(double, ageingPropabilityOffset, 1.0);

            /* indicate that we are want a random value connected to each output */
            addOption(bool, useRandomBiases, false);

            /* indicate that we are alowing insertion of neurons into BIAS neurons */
            addOption(bool, allowNeuronIntoBiasConnectionInsert, false);
            
            /* indicates that the network should be updated layer by layer */
            addOption(bool, layeredNetwork, true);

            /* indicates that the fitness should always be recalculated for each organism */
            addOption(bool, alwaysRecalculateFitness, false);

            /* number of threads */
            addOption(unsigned, numThreads, 1);

            /* the perturbe size range start and end */
            addOption(double, perturbeSizeRangeStart, -0.1)
            addOption(double, perturbeSizeRangeEnd, 0.1)

            /* the size range for seting sizes during mutation */
            addOption(double, mutationSetSizeRangeStart, 0.0)
            addOption(double, mutationSetSizeRangeEnd, 6.283185307179586l)

            /* the size range for creating a ConnectionGene */
            addOption(double, creationSizeRangeStart, 0.0) 
            addOption(double, creationSizeRangeEnd, 6.283185307179586)

            /* the max size range for pertubing size during mutation */
            addOption(double, mutationMaxPertubeSizeRangeStart, 0.0) 
            addOption(double, mutationMaxPertubeSizeRangeEnd, 6.283185307179586)

            /* the propability of setting a structure size to 
             * a new random value during mutation 
             */
            addOption(double, hardMutationPropabilitySize, 0.1)

            /* the desired number of stuctures */
            addOption(unsigned, wantedNumberOfStructures, 30);

            /* the minimum possible fittness */
            addOption(double, minFitness, 0.00000001);

            /* the size of one hexagon on the energy field */
            addOption(double, energyFieldSize, 30);

            /* terrain scale controls the frequency of the noise */
            addOption(double, terrainScale, 0.4)

            /* terrain amplitude controls the height variation */
            addOption(double, terrainAmplitude, 1.0)

            /* terrain seed for random generation */
            addOption(int, terrainSeed, 42)

            /* number of octaves for fBm */
            addOption(int, terrainOctaves, 4)

            /* persistence controls how much each octave contributes */
            addOption(double, terrainPersistence, 0.5)

            /* lacunarity controls frequency increase with each octave */
            addOption(double, terrainLacunarity, 2.0)

            /* base height of the terrain */
            addOption(double, terrainBaseHeight, 0.0)

            /* flatness of the terrain */
            addOption(double, terrainFlatness, 1.1)

            /* small-scale bumpiness */
            addOption(double, terrainBumpiness, 0.1)

        };
    }
}

#undef addOption

#endif /* __OPTS_H__ */
