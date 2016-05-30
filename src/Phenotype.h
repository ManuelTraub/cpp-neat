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

            /* the genotype of this */
            const Genotype *const genome;

        public:

            /* returns the neuronLayers of this */
            vector<vector<Neuron *>> getLayers() const { return neuronLayers; }

            /* create a new Phenotype out of the given Genotype */
            Phenotype(Genotype *genome); 

            /* calculate the output for a given input */
            vector<double> run(vector<double> inputs);

            /* export this as an svg image */
            void saveAsSVG(string fname);

            /* destructor */
            ~Phenotype();
    };
}

#endif /* __PHENOTYPE_H__ */
