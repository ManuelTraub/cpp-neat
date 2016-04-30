#include "Gene.h"

using namespace NEAT;

/* the global id counter */
uint64_t ConnectionGene::idCounter = 0;

/* global lock to prevent acces to the idCounter */
pthread_mutex_t ConnectionGene::mutex = PTHREAD_MUTEX_INITIALIZER;

/* global random value to mutate this */
rand128_t *ConnectionGene::rand = new_rand128_t(time(NULL));
