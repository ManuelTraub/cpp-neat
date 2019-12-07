/**
 * Abstract Gene representation within NEAVC
 * each Gene has an historical id to track back
 * its origin
 */
#ifndef __GENE_H__
#define __GENE_H__

#include <inttypes.h>
#include <pthread.h>
#include <assert.h>
#include <vector>
#include "../utils/utils.h"
#include "../utils/TOGrid.h"
#include "GenerationCounter.h"

using std::vector;

namespace NEAVC {

    namespace AI {

        /* An abstract Gene which has an historical id so as an generation number 
         * naming the generation it was invented or mutated.
         */
        class Gene {

            private:

                /* the global id counter */
                static uint64_t idCounter; 

                /* global lock to prevent acces to the idCounter */
                static pthread_mutex_t mutex;

                /* the historical id of this */
                uint64_t id;

            protected:

                /* global random value to mutate this */
                static rand64_t *rand;

                /* the generation this was invented or mutated */
                uint64_t generation;

                /* should save the child of this */
                virtual bool saveChild(const int) const { return false; }

                /* loads the base parts of this */
                bool loadBase(const int fd);

            public:
                
                /* creates a new Gene */
                Gene() {

                    pthread_mutex_lock(&mutex);
                    this->generation = GenerationCounter::get();
                    this->id         = idCounter++;
                    pthread_mutex_unlock(&mutex);

                    assert(id != UINT64_MAX);
                    assert(generation <= GenerationCounter::get());
                }

                /* returns the xRaster of this */
                uint64_t getID() const { return id; }

                /* returns the generation number this was invented or mutated */
                uint64_t getGeneration() const { 
                    assert(generation <= GenerationCounter::get());
                    return generation; 
                }

                /* returns the age of this */
                uint64_t age() const {
                    assert(generation <= GenerationCounter::get());
                    return GenerationCounter::get() - generation;
                }

                /* saves this */
                bool save(const int fd) const;
        };

        /* the different types of neurons (input output or hidden) */
        enum class NeuronType { BIAS = 0, INPUT = 1, HIDDEN = 2, OUTPUT = 3, RAND_BIAS = 4 };

        /* the different Netactivation functions possible */
        enum class ActivationFunction { SIGMOID, TANH, RELU, SINE, LOG, SQRT, ABS, POW, EXP, ADD, MULT, GAUSSIAN, SPIKING, VOLTAGE, EXP_SMOOTH, LENGTH };


        /**
         * A Neuron Gene representing a neuron within a neural network
         * NeuronGenes are enumerated within a genome to unique identify them.
         * To be able to detect same Neurons from different genomes, 
         * the Neurons are placed on a 2D raster giving them a mostly unique 2D possition
         */
        class NeuronGene : public Gene {

            private:

                /* the neuron type of this */
                NeuronType type;

                /* the ActivationFunction of this */
                ActivationFunction function;

                /* the layer id of this (greater means nearer to the output layer) */
                uint64_t layerID;

                /* the gradient of this */
                double gradient;

                /* should save the child of this */
                virtual bool saveChild(const int fd) const;

            public:

                /* returns the type of this */
                NeuronType getType() const { return type; }

                /* returns the ActivationFunction of this */
                ActivationFunction getFunction() const { return function; }

                /* set the ActivationFunction of this */
                void setFunction(const ActivationFunction function) {
                    this->function = function;
                }

                /* returns the layer if of this */
                uint64_t getLayerID() const { return layerID; }

                /* creats a new NeuronGeneID */
                NeuronGene(
                    const NeuronType type, 
                    const ActivationFunction function, 
                    const uint64_t layerID
                ) : 
                    type(type), function(function), layerID(layerID), gradient(4.924273) { 

                    assert(type == NeuronType::INPUT  || 
                           type == NeuronType::OUTPUT ||
                           type == NeuronType::BIAS   ||
                           type == NeuronType::RAND_BIAS);
                }

                /* loads the NeuronGene from the given file */
                NeuronGene(const int fd);

                /* returns wether a new node can be created between the two given nodes */
                static bool canBeInserted(const NeuronGene &gene1, const NeuronGene &gene2);

                /* create a new node between two nodes */
                NeuronGene(
                    const NeuronGene &gene1, 
                    const NeuronGene &gene2,
                    const ActivationFunction function
                ) : type(NeuronType::HIDDEN),
                    function(function),
                    layerID(middle(gene1.layerID, gene2.layerID)), gradient(4.924273) {

                    assert(canBeInserted(gene1, gene2));
                }

                /* compare operator */
                bool operator == (const NeuronGene &other) const {
                    assert((getID() == other.getID() && layerID == other.layerID) ||
                           getID() != other.getID());

                    return getID() == other.getID();
                }
                
                /* mutates the gradient of this */
                void mutateGradient() {
                    assert(gradient >= 0);
                    gradient *= rand_range_d64(rand, 0.0, 2.0);
                }

                /* mutates the ActivationFunction of this */
                void mutateFunction() {
                    function = ActivationFunction(rand64(rand) % int(ActivationFunction::SPIKING));
                }

                /* returns the gradient of this */
                double getGradient() const { return gradient; }

                /* sets the gradient of this */
                void setGradient(const double gradient) { 
                    generation     = GenerationCounter::get();
                    this->gradient = gradient; 
                }
        };

        /**
         * A Gene representing a connection between two neurons
         */
        class ConnectionGene : public Gene {
            
            private:

                /* the input and output neuronal gene */
                NeuronGene inputNeuron, outputNeuron;

                /* the weight of this connection */
                double weight;

                /* bool indicates if this gene is expressed within the phenotype */
                bool enabled;

                /* should save the child of this */
                virtual bool saveChild(const int fd) const;

            public: 

                /* creats a new Gene with two given neuron ids */
                ConnectionGene(const NeuronGene inputNeuron, 
                               const NeuronGene outputNeuron,
                               double weight = rand_range_d64(rand, -1, 1)) : 
                               inputNeuron(inputNeuron),
                               outputNeuron(outputNeuron),
                               weight(weight),
                               enabled(true) { }

                /* loads the ConnectionGene from the given file */
                ConnectionGene(const int fd);

                /* returns the inputNeuron of this */
                NeuronGene getInput() const { return inputNeuron; }

                /* returns the outputNeuron of this */
                NeuronGene getOutput() const { return outputNeuron; }
                
                /* returns the weight of this */
                double getWeight() const { return weight; }

                /* set the weight */
                void setWeight(const double weight) {
                    generation   = GenerationCounter::get();
                    this->weight = weight;
                }

                /* disables this gene */
                void disable() { 
                    enabled    = false; 
                    generation = GenerationCounter::get();
                }

                /* enable this gene */
                void enable() { 
                    enabled    = true; 
                    generation = GenerationCounter::get();
                }

                /* returns if this is enabled */
                bool isEnabled() const { return enabled; }

                /* resets the waight of this to a random value */
                void mutateSet(const vector<double> weightRange) { 
                    weight = rand_range_d64(rand, weightRange[0], weightRange[1]); 
                    generation = GenerationCounter::get();
                }

                /* uniformly perturbe the weight of this */
                void mutatePertube(const vector<double> weightRange,
                                   const vector<double> maxWeightRange) {

                    weight += rand_range_d64(rand, weightRange[0], weightRange[1]); 

                    if (weight < maxWeightRange[0]) weight = maxWeightRange[0];
                    if (weight > maxWeightRange[1]) weight = maxWeightRange[1];

                    generation = GenerationCounter::get();
                }

                /* compares to genes based on their 2D location */
                bool operator == (const ConnectionGene &other) const {
                    return inputNeuron  == other.inputNeuron && 
                           outputNeuron == other.outputNeuron;
                }

                /* returns thrue if the NeuronGene can be splitted */
                bool canBeSplitted() const {
                    return NeuronGene::canBeInserted(inputNeuron, outputNeuron);
                }

                /* returns a new NeuronGene spliting this */
                NeuronGene split(const ActivationFunction function) const {
                    return NeuronGene(inputNeuron, outputNeuron, function);
                }
        };

        /* the different types of cellular tissues */
        enum class TissueType { TISSUE, NEURON, SENSOR, MUSCLE };

        /**
         * A StructureGene encodes for a specific cellular tissue 
         * at a specific possition in 3D space
         */
        class StructureGene : public Gene {
            
            private:

                /* the tissue type of this gene */
                TissueType type;

                /* the possition in 3D space */
                int64_t x, y, z;

                /* the size of this */
                double size;

                /* should save the child of this */
                virtual bool saveChild(const int fd) const;

            public:

                /* getter method for the type of this */
                TissueType getType() const { return type; }

                /* sets the tissues type of this */
                void setType(const TissueType type) { 
                    this->type = type; 
                    generation = GenerationCounter::get();
                }

                /* returns the possition in 3D space of this */
                TOGridIndex getIndex() const { return TOGridIndex(x, y, z); }

                /* returns the size of this */
                double getSize() const { return size; }

                /* sets the size of this */
                void setSize(const double size) { this->size = size; }

                /* resets the size of this to a random value */
                void mutateSet(const vector<double> sizeRange) { 
                    size = rand_range_d64(rand, sizeRange[0], sizeRange[1]); 
                    generation = GenerationCounter::get();
                }

                /* uniformly perturbe the weight of this */
                void mutatePertube(const vector<double> sizeRange,
                                   const vector<double> maxSizeRange) {

                    size += rand_range_d64(rand, sizeRange[0], sizeRange[1]); 

                    if (size < maxSizeRange[0]) size = maxSizeRange[0];
                    if (size > maxSizeRange[1]) size = maxSizeRange[1];

                    generation = GenerationCounter::get();
                }
                

                /* constructor of this */
                StructureGene(
                    const TissueType type, 
                    const int64_t x,
                    const int64_t y,
                    const int64_t z,
                    const double size
                ) : type(type), x(x), y(y), z(z), size(size)  { }

                /* loads the StructureGene from the given file */
                StructureGene(const int fd);

        };

        /* the sensor effector types of this */
        enum class SensorEffector { ENERGY_SENSOR, TOUCH_SENSOR, VISION_SENSOR, MUSCLE_EFFECTOR };

        /* linking an sensor / effector (NeuronGene) */
        class SEGeneLink {

            private:

                /* the NeuronGene of this */
                NeuronGene neuron;

                /* the SensorEffector type of this */
                SensorEffector type;

                /* the structural possition this belongs to */
                int64_t x, y, z;

                /* extra dimensional index */
                uint64_t n;

            public:

                /* returns the neuron of this */
                NeuronGene getNeuron() const { return neuron; }

                /* returns the type of this */
                SensorEffector getType() const {  return type; }

                /* returns teh index of this */
                const TOGridIndex getIndex() const { return TOGridIndex(x, y, z, n); }

                /* constructor */
                SEGeneLink(
                    const NeuronGene &neuron, 
                    const SensorEffector type, 
                    const TOGridIndex index = TOGridIndex()
                ) :
                    neuron(neuron), 
                    type(type),
                    x(index.getX()),
                    y(index.getY()),
                    z(index.getZ()),
                    n(index.getN()) { 

                    assert(neuron.getType() == NeuronType::INPUT || 
                           neuron.getType() == NeuronType::OUTPUT);

                }
        };
    }
}

#endif /* __GENE_H__ */
