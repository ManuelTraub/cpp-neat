#include "Gene.h"
#include <unistd.h>

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

/* saves this to the given file descriptor */
bool Gene::save(const int fd) const {
    
    if (write(fd, &id, sizeof(uint64_t)) != sizeof(uint64_t))
        return false;

    if (write(fd, &generation, sizeof(uint64_t)) != sizeof(uint64_t))
        return false;

    return this->saveChild(fd);
}

/* loads the base parts of this */
bool Gene::loadBase(const int fd) {
    
    if (read(fd, &id, sizeof(uint64_t)) != sizeof(uint64_t))
        return true;

    if (read(fd, &generation, sizeof(uint64_t)) != sizeof(uint64_t))
        return true;

    if (id >= idCounter)
        idCounter = id + 1;

    return true;
}

/* should save the child of this */
bool NeuronGene::saveChild(const int fd) const {

    if (type == NeuronType::BIAS) {
        if (write(fd, "b", 1) != 1) return false;
    } else if (type == NeuronType::INPUT) {
        if (write(fd, "i", 1) != 1) return false;
    } else if (type == NeuronType::HIDDEN) {
        if (write(fd, "h", 1) != 1) return false;
    } else if (type == NeuronType::OUTPUT) {
        if (write(fd, "o", 1) != 1) return false;
    } else {
        if (write(fd, "e", 1) != 1) return false;
    }

    if (write(fd, &layerID, sizeof(uint64_t)) != sizeof(uint64_t))
        return false;

    if (write(fd, &gradient, sizeof(double)) != sizeof(double))
        return false;

    return true;
}

/* loads the NeuronGene from the given file */
NeuronGene::NeuronGene(const int fd) {
    
    if (!this->loadBase(fd))
        throw "IO Exception";

    char cType = '\0';
    if (read(fd, &cType, 1) != 1)
        throw "IO Exception";

    if (cType == 'b')
        type = NeuronType::BIAS;
    else if (cType == 'i')
        type = NeuronType::INPUT;
    else if (cType == 'h')
        type = NeuronType::HIDDEN;
    else if (cType == 'o')
        type = NeuronType::OUTPUT;
    else 
        throw "IO Exception";

    if (read(fd, &layerID, sizeof(uint64_t)) != sizeof(uint64_t))
        throw "IO Exception";

    if (read(fd, &gradient, sizeof(double)) != sizeof(double))
        throw "IO Exception";
}

/* should save the child of this */
bool ConnectionGene::saveChild(const int fd) const {

    if (!inputNeuron.save(fd)) 
        return false;

    if (!outputNeuron.save(fd)) 
        return false;

    if (write(fd, &weight, sizeof(double)) != sizeof(double))
        return false;

    if (write(fd, &enabled, sizeof(bool)) != sizeof(bool))
        return false;

    return true;
}

/* loads the ConnectionGene from the given file */
ConnectionGene::ConnectionGene(const int fd) : 
    inputNeuron(NeuronType::INPUT, 0), 
    outputNeuron(NeuronType::OUTPUT, UINT64_MAX) 
{

    if (!this->loadBase(fd))
        throw "IO Exception";

    inputNeuron  = NeuronGene(fd);
    outputNeuron = NeuronGene(fd);
    
    if (read(fd, &weight, sizeof(double)) != sizeof(double))
        throw "IO Exception";

    if (read(fd, &enabled, sizeof(bool)) != sizeof(bool))
        throw "IO Exception";
}

/* should save the child of this */
bool StructureGene::saveChild(const int fd) const {

    if (type == TissueType::TISSUE) {
        if (write(fd, "t", 1) != 1) return false;
    } else if (type == TissueType::NEURON) {
        if (write(fd, "n", 1) != 1) return false;
    } else if (type == TissueType::SENSOR) {
        if (write(fd, "s", 1) != 1) return false;
    } else if (type == TissueType::MUSCLE) {
        if (write(fd, "m", 1) != 1) return false;
    } else {
        if (write(fd, "e", 1) != 1) return false;
    }

    if (write(fd, &x, sizeof(int64_t)) != sizeof(int64_t))
        return false;

    if (write(fd, &y, sizeof(int64_t)) != sizeof(int64_t))
        return false;

    if (write(fd, &z, sizeof(int64_t)) != sizeof(int64_t))
        return false;

    return true;
}


/* loads the StructureGene from the given file */
StructureGene::StructureGene(const int fd) {

    if (!this->loadBase(fd))
        throw "IO Exception";

    char cType = '\0';
    if (read(fd, &cType, 1) != 1)
        throw "IO Exception";

    if (cType == 't')
        type = TissueType::TISSUE;
    else if (cType == 'n')
        type = TissueType::NEURON;
    else if (cType == 's')
        type = TissueType::SENSOR;
    else if (cType == 'm')
        type = TissueType::MUSCLE;
    else 
        throw "IO Exception";

    if (read(fd, &x, sizeof(int64_t)) != sizeof(int64_t))
        throw "IO Exception";

    if (read(fd, &y, sizeof(int64_t)) != sizeof(int64_t))
        throw "IO Exception";

    if (read(fd, &z, sizeof(int64_t)) != sizeof(int64_t))
        throw "IO Exception";
}
