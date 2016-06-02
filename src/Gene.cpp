#include "Gene.h"

using namespace NEAT;

/* the global id counter */
uint64_t NeuronGene::idCounter = 0; 

/* global lock to prevent acces to the idCounter */
pthread_mutex_t NeuronGene::mutex = PTHREAD_MUTEX_INITIALIZER;

/* returns wether a new node can be created between the two given nodes */
bool NeuronGene::canBeInserted(const NeuronGene &gene1, const NeuronGene &gene2) {
    assert(gene1.id != UINT64_MAX && gene2.id != UINT64_MAX);
    return absDiff(gene1.layerID, gene2.layerID) > 1;
}

/* the global id counter */
uint64_t ConnectionGene::idCounter = 0;

/* global lock to prevent acces to the idCounter */
pthread_mutex_t ConnectionGene::mutex = PTHREAD_MUTEX_INITIALIZER;

/* global random value to mutate this */
rand64_t *ConnectionGene::rand = new_rand64_t(time(NULL));
