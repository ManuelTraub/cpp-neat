/**
 * Abstract Genotype encoding a Individual within NEAT
 */
#ifndef __GENOTYPE_H__
#define __GENOTYPE_H__

#include <vector>

using std::vector;

namespace NEAT {

    /* forward declaration of the Phenotype */
    class Phenotype;
    
    class Genotype {
        
        /* let the Phenotype access the private members fo the Genotype */
        friend class Phenotype;
        
        private:
            
            /* the neurons of this sorted by layer */
            vector<vector<NeuronGene>> neurons;

            /* the ConnectionGenes of this sorted first by layer
             * and second by theyr historical id */
            vector<vector<ConnectionGenes>> connections;
    }
}

#endif /* __GENOTYPE_H__ */
