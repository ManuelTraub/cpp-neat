/**
 * Abstract Phenotype encoding a Individual within NEAT
 */
#ifndef __PHENOTYPE_H__
#define __PHENOTYPE_H__

#include "Genotype.h"
#include "Neuron.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

namespace NEAT {
    
    /**
     * Represennst a Neural network within NEAT
     */
    class Phenotype {
        
        private:

            /* The Neurons of this sorted into layers 
             * to sequentaly calculate the output of this neural network */
            vector<vector<Neuron *>> neuronLayers;

            /* the bias neuron */
            Neuron *bias;

            /* the genotype of this */
            const Genotype &genome;

        public:

            /* returns the neuronLayers of this */
            vector<vector<Neuron *>> getLayers() const { return neuronLayers; }

            /* create a new Phenotype out of the given Genotype */
            Phenotype(const Genotype &genome); 

            /* calculate the output for a given input */
            vector<double> run(vector<double> inputs);

            /* returns the index of the output with tha higest value */
            unsigned maxOutput(vector<double> inputs);

            /* calculaes the quadratic error for the given data set */
            double calculateError(vector<vector<double>> inputs, vector<vector<double>> outputs);

            /* calculaes the maximum derivation for a single output from the training data */
            double maxDerivation(vector<vector<double>> inputs, vector<vector<double>> outputs);

            /* export this as an svg image */
            void saveAsSVG(string fname);

            /* destructor */
            ~Phenotype();
    };
}

#endif /* __PHENOTYPE_H__ */
