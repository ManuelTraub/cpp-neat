/**
 * Abstract Phenotype encoding a Individual within NEAVC
 */
#ifndef __PHENOTYPE_H__
#define __PHENOTYPE_H__

#include "Genotype.h"
#include "Neuron.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

namespace NEAVC {
    
    namespace AI {
        
        /**
         * Represennst a Neural network within NEAVC
         */
        class Phenotype {
            
            private:

                /* reference to the options of the whole population */
                Opts &opts;

                /* The Neurons of this sorted into layers 
                 * to sequentaly calculate the output of this neural network */
                vector<vector<Neuron *>> neuronLayers;

                /* efficient mapping of Neurons to theyr gene id */
                unordered_map<uint64_t, Neuron *> neuronMap;

                /* the bias neuron */
                Neuron *bias;

                /* the random bias neurons */
                vector<Neuron *> randomBiases;

                /* random generators for the random bias neurons */
                vector<rand64_t *> randomBiasGenerators;

                /* the genotype of this */
                const Genotype &genome;

                /* indicates taht a firing pattern should be saved (spiking neurons) */
                const bool storeFirings;

                /* the stored firings */
                vector<vector<bool>> firings;

            public:

                /* returns the neuronLayers of this */
                vector<vector<Neuron *>> getLayers() const { return neuronLayers; }

                /* returns the neurons of this mapped to gene ids */
                unordered_map<uint64_t, Neuron *> getIDMap() const { return neuronMap; }

                /* create a new Phenotype out of the given Genotype */
                Phenotype(const Genotype &genome, Opts &opts, bool storeFirings = false); 

                /* resets the network and intorduces random dynamics */
                void initNet(unsigned cycles = 200);

                /* calculate the output for a given input */
                vector<double> run(vector<double> inputs);

                /* calculate the output for a given input within a single pass trough the network */
                vector<double> runSinglePass(vector<double> inputs);

                /* calculate the output of a spiking neural network for a given input */
                vector<double> runSpiking(vector<double> inputs);

                /* returns the index of the output with tha higest value */
                unsigned maxOutput(vector<double> inputs);

                /* calculaes the quadratic error for the given data set */
                double calculateError(vector<vector<double>> inputs, vector<vector<double>> outputs);

                /* calculaes the maximum derivation for a single output from the training data */
                double maxDerivation(vector<vector<double>> inputs, vector<vector<double>> outputs);

                /* export this as an svg image */
                void saveAsSVG(string fname);

                /* create an image from a frigin pattern */
                void createFiringPattern(std::string fileName);

                /* prints the weights of this */
                void printWeights();

                /* resets all neurons of this to zero */
                void reset() {
                    for (auto &e: neuronMap)
                        e.second->reset();
                }

                /* destructor */
                ~Phenotype();
        };
    }
}

#endif /* __PHENOTYPE_H__ */
