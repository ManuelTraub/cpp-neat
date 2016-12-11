#include "Gene.h"

using namespace NEAVC;
using namespace AI;

/* the global id counter */
uint64_t Gene::idCounter = 0; 

/* global lock to prevent acces to the idCounter */
pthread_mutex_t Gene::mutex = PTHREAD_MUTEX_INITIALIZER;

/* returns wether a new node can be created between the two given nodes */
bool NeuronGene::canBeInserted(const NeuronGene &gene1, const NeuronGene &gene2) {

    /* alow insertion in the same layer and also
     * between the same layers if ley are more then one id apart */
    return absDiff(gene1.layerID, gene2.layerID) > 1 || gene1.layerID == gene2.layerID;
}

/* global random value to mutate Genes */
rand64_t *Gene::rand = new_rand64_t();
