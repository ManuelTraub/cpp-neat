/**
 * Abstract Phenotype encoding a Individual within NEAT
 */
#ifndef __PHENOTYPE_H__
#define __PHENOTYPE_H__

#include "Genotype.h"
#include "Neuron.h"
#include <vector>

using std::vector;

namespace NEAT {
    
    /**
     * Represennst a Neural network within NEAT
     */
    class Phenotype {
        
        private:

            /* The Neurons of this sorted into layers 
             * to sequentaly calculate the output of this neural network */
            vector<vector<Neuron *>> neuronLayers;

        public:

            /* create a new Phenotype out of the given Genotype */
            Phenotype(Genotype *genome); 

            /* calculate the output for a given input */
            vector<double> run(vector<double> inputs);
    };
}

#endif /* __PHENOTYPE_H__ */
