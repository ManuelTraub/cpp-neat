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
        if (write(fd, "E", 1) != 1) return false;
    }

    if (function == ActivationFunction::SIGMOID) {
        if (write(fd, "s", 1) != 1) return false;
    } else if (function == ActivationFunction::TANH) {
        if (write(fd, "t", 1) != 1) return false;
    } else if (function == ActivationFunction::RELU) {
        if (write(fd, "r", 1) != 1) return false;
    } else if (function == ActivationFunction::SINE) {
        if (write(fd, "e", 1) != 1) return false;
    } else if (function == ActivationFunction::SPIKING) {
        if (write(fd, "i", 1) != 1) return false;
    } else {
        if (write(fd, "E", 1) != 1) return false;
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

    char cFunction = '\0';
    if (read(fd, &cFunction, 1) != 1)
        throw "IO Exception";

    if (cFunction == 's')
        function = ActivationFunction::SIGMOID;
    else if (cFunction == 't')
        function = ActivationFunction::TANH;
    else if (cFunction == 'r')
        function = ActivationFunction::RELU;
    else if (cFunction == 'e')
        function = ActivationFunction::SINE;
    else if (cFunction == 'i')
        function = ActivationFunction::SPIKING;
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
    inputNeuron(NeuronType::INPUT, ActivationFunction::SIGMOID, 0), 
    outputNeuron(NeuronType::OUTPUT, ActivationFunction::SIGMOID, UINT64_MAX) 
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
        if (write(fd, "E", 1) != 1) return false;
    }

    if (write(fd, &x, sizeof(int64_t)) != sizeof(int64_t))
        return false;

    if (write(fd, &y, sizeof(int64_t)) != sizeof(int64_t))
        return false;

    if (write(fd, &z, sizeof(int64_t)) != sizeof(int64_t))
        return false;

    if (write(fd, &size, sizeof(double)) != sizeof(double))
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

    if (read(fd, &size, sizeof(double)) != sizeof(double))
        throw "IO Exception";
}

/* saves this */
bool SEGeneLink::save(const int fd) const {

    if (!neuron.save(fd)) 
        return false;

    if (type == SensorEffector::ENERGY_SENSOR) {
        if (write(fd, "e", 1) != 1) return false;
    } else if (type == SensorEffector::TOUCH_SENSOR) {
        if (write(fd, "t", 1) != 1) return false;
    } else if (type == SensorEffector::VISION_SENSOR) {
        if (write(fd, "v", 1) != 1) return false;
    } else if (type == SensorEffector::MUSCLE_EFFECTOR) {
        if (write(fd, "m", 1) != 1) return false;
    } else {
        if (write(fd, "E", 1) != 1) return false;
    }

    if (write(fd, &x, sizeof(int64_t)) != sizeof(int64_t))
        return false;

    if (write(fd, &y, sizeof(int64_t)) != sizeof(int64_t))
        return false;

    if (write(fd, &z, sizeof(int64_t)) != sizeof(int64_t))
        return false;

    if (write(fd, &n, sizeof(int64_t)) != sizeof(int64_t))
        return false;
    
    return true;
}

/* loads the SEGeneLink from the given file */
SEGeneLink::SEGeneLink(const int fd): 
    neuron(NeuronType::INPUT, ActivationFunction::SIGMOID, 0)
{
    
    neuron = NeuronGene(fd);

    char cType = '\0';
    if (read(fd, &cType, 1) != 1)
        throw "IO Exception";

    if (cType == 'e')
        type = SensorEffector::ENERGY_SENSOR;
    else if (cType == 't')
        type = SensorEffector::TOUCH_SENSOR;
    else if (cType == 'v')
        type = SensorEffector::VISION_SENSOR;
    else if (cType == 'm')
        type = SensorEffector::MUSCLE_EFFECTOR;
    else 
        throw "IO Exception";

    if (read(fd, &x, sizeof(int64_t)) != sizeof(int64_t))
        throw "IO Exception";

    if (read(fd, &y, sizeof(int64_t)) != sizeof(int64_t))
        throw "IO Exception";

    if (read(fd, &z, sizeof(int64_t)) != sizeof(int64_t))
        throw "IO Exception";

    if (read(fd, &n, sizeof(int64_t)) != sizeof(int64_t))
        throw "IO Exception";

}

