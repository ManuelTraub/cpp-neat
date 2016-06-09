/**
 * Abstract Gene representation within NEAT
 * each Gene has an historical id to track back
 * its origin
 */
#ifndef __GENE_H__
#define __GENE_H__

#include <inttypes.h>
#include <pthread.h>
#include <assert.h>
#include "utils.h"
#include "QuadTree.h"

namespace NEAT {

    /* the different types of neurons (input output or hidden) */
    enum class NeuronType { BIAS = 0, INPUT = 1, HIDDEN = 2, OUTPUT = 3 };

    /**
     * A Neuron Gene representing a neuron within a neural network
     * NeuronGenes are enumerated within a genome to unique identify them.
     * To be able to detect same Neurons from different genomes, 
     * the Neurons are placed on a 2D raster giving them a mostly unique 2D possition
     */
    class NeuronGene {

        private:

            /* the global id counter */
            static uint64_t idCounter; 

            /* global lock to prevent acces to the idCounter */
            static pthread_mutex_t mutex;

            /* the neuron type of this */
            NeuronType type;

            /* the historical id of this */
            uint64_t id;

            /* the layer id of this (greater means nearer to the output layer) */
            uint64_t layerID;

        public:

            /* returns the type of this */
            NeuronType getType() const { assert(id != UINT64_MAX); return type; }

            /* returns the xRaster of this */
            uint64_t getID() const { assert(id != UINT64_MAX); return id; }

            /* returns the layer if of this */
            uint64_t getLayerID() const { assert(id != UINT64_MAX); return layerID; }

            /* creats a new NeuronGeneID */
            NeuronGene(const NeuronType type, const uint64_t layerID) : 
                       type(type), layerID(layerID) { 

                assert(type == NeuronType::INPUT  || 
                       type == NeuronType::OUTPUT ||
                       type == NeuronType::BIAS);

                pthread_mutex_lock(&mutex);
                this->id = idCounter++;
                pthread_mutex_unlock(&mutex);
                assert(id != UINT64_MAX);
            }

            /* returns wether a new node can be created between the two given nodes */
            static bool canBeInserted(const NeuronGene &gene1, const NeuronGene &gene2);

            /* create a new node between two nodes */
            NeuronGene(
                const NeuronGene &gene1, 
                const NeuronGene &gene2
            ) : type(NeuronType::HIDDEN),
                layerID(middle(gene1.layerID, gene2.layerID)) {

                assert(canBeInserted(gene1, gene2));

                pthread_mutex_lock(&mutex);
                this->id = idCounter++;
                pthread_mutex_unlock(&mutex);

                assert(id != UINT64_MAX);
            }

            /* compare operator */
            bool operator == (const NeuronGene &other) const {
                assert(id != UINT64_MAX && other.id != UINT64_MAX);
                assert((id == other.id && layerID == other.layerID) || id != other.layerID);
                return id == other.id;
            }

            /* generate a dummy placeholder gene */
            NeuronGene() : type(NeuronType::HIDDEN), id(UINT64_MAX), layerID(UINT64_MAX) { }
    };

    /**
     * A Gene representing a connection between two neurons
     */
    class ConnectionGene {
        
        private:

            /* the global id counter */
            static uint64_t idCounter; 

            /* global lock to prevent acces to the idCounter */
            static pthread_mutex_t mutex;

            /* global random value to mutate this */
            static rand64_t *rand;

            /* the input and output neuronal gene */
            NeuronGene inputNeuron, outputNeuron;

            /* the weight of this connection */
            double weight;

            /* bool indicates if this gene is expressed within the phenotype */
            bool enabled;

            /* the historical id if this */
            uint64_t id;

        public: 

            /* generate a dummy placeholder gene */
            ConnectionGene() : weight(0), enabled(false), id(UINT64_MAX) { }

            /* creats a new Gene with two given neuron ids */
            ConnectionGene(const NeuronGene inputNeuron, 
                           const NeuronGene outputNeuron,
                           double weight = rand_range_d64(rand, -1.0, 1.0)) : 
                           inputNeuron(inputNeuron),
                           outputNeuron(outputNeuron),
                           weight(weight),
                           enabled(true) {
                
                pthread_mutex_lock(&mutex);
                this->id = idCounter++;
                pthread_mutex_unlock(&mutex);

                assert(id != UINT64_MAX);
            }

            /* returns the historical id of this */
            uint64_t getID() const { assert(id != UINT64_MAX); return id; }

            /* returns the inputNeuron of this */
            NeuronGene getInput() const { assert(id != UINT64_MAX); return inputNeuron; }

            /* returns the outputNeuron of this */
            NeuronGene getOutput() const { assert(id != UINT64_MAX); return outputNeuron; }
            
            /* returns the weight of this */
            double getWeight() const { assert(id != UINT64_MAX); return weight; }

            /* set the weight */
            void setWeight(const double weight) {
                assert(weight >= -1.0 && weight <= 1.0);
                this->weight = weight;
                assert(id != UINT64_MAX); 
            }

            /* disables this gene */
            void disable() { assert(id != UINT64_MAX && enabled); enabled = false; }

            /* returns if this is enabled */
            bool isEnabled() const { assert(id != UINT64_MAX); return enabled; }

            /* resets the waight of this to a random value */
            void mutate() { assert(id != UINT64_MAX); weight = rand_range_d64(rand, -1.0, 1.0); }

            /* compares to genes based on their 2D location */
            bool operator == (const ConnectionGene &other) const {
                assert(id != UINT64_MAX); 
                return inputNeuron  == other.inputNeuron && 
                       outputNeuron == other.outputNeuron;
            }

            /* returns thrue if the NeuronGene can be splitted */
            bool canBeSplitted() const {
                assert(id != UINT64_MAX); 
                return NeuronGene::canBeInserted(inputNeuron, outputNeuron);
            }

            /* returns a new NeuronGene spliting this */
            NeuronGene split() const {
                assert(id != UINT64_MAX);  
                return NeuronGene(inputNeuron, outputNeuron);
            }
    };
}

#endif /* __GENE_H__ */
