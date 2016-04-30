/**
 * Abstract Phenotype encoding a Individual within NEAT
 */
#ifndef __PHENOTYPE_H__
#define __PHENOTYPE_H__

#include "Genotype.h"
#include <vector>

using std::vector;

namespace NEAT {
    
    /**
     * Represennst anNeural network within NEAT
     */
    class Phenotype {
        
        private:

            /* the Genotype represented by this Phenotype */
            Genotype &genome;

        public:

            /* create a new Phenotype out of the given Genotype */
            Phenotype(Genotype &genome) : genome(genome) { }

            /* calculate the output for a given input */
            vector<double> run(vector<double> inputs);
    };
}

#endif /* __PHENOTYPE_H__ */
