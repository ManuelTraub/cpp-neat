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

namespace NEAT {

    /* the different types of neurons (input output or hidden) */
    enum class NeuronType { INPUT, OUTPUT, HIDDEN, BIAS };

    /**
     * A Neuron Gene representing a neuron within a neural network
     * NeuronGenes are enumerated within a genome to unique identify them.
     * To be able to detect same Neurons from different genomes, 
     * the Neurons are placed on a 2D raster giving them a mostly unique 2D possition
     */
    struct NeuronGene {

        /* the neuron type of this */
        const NeuronType type;

        /* the x raster index */
        const uint64_t xRaster;

        /* the y raster index */
        const uint64_t yRaster;

        /* creats a new NeuronGeneID */
        NeuronGene(
            const NeuronType type,
            const uint64_t xRaster, 
            const uint64_t yRaster
        ) : type(type), xRaster(xRaster), yRaster(yRaster) { }

        /* returns wether a new node can be created between the two given nodes */
        bool canBeInserted(const NeuronGene &gene1, const NeuronGene &gene2) {
            return abs(gene1.xRaster - gene2.xRaster) > 1 ||
                   abs(gene1.yRaster - gene2.yRaster) > 1;
        }

        /* create a new node between two nodes */
        NeuronGene(
            const NeuronType type,
            const NeuronGene &gene1, 
            const NeuronGene &gene2
        ) : type(type), 
            xRaster(gene1.xRaster + (gene2.xRaster - gene1.xRaster)/2),
            yRaster(gene1.yRaster + (gene2.yRaster - gene1.yRaster)/2) {

            assert(canBeInserted(gene1, gene2));
        }

        /* compare operator */
        bool operator == (const NeuronGene &other) const {
            return xRaster == other.xRaster && 
                   yRaster == other.yRaster;
        }
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

            /* bool indicates if this gene is expressed within the phenotype */
            bool enabled;

            /* the weight of this connection */
            double weight;

            /* private constructor that asigns the historical id */
            ConnectionGene(const NeuronGene inputNeuron, 
                           const NeuronGene outputNeuron,
                           double weight,
                           uint64_t id) :
                           weight(weight),
                           id(id),
                           inputNeuron(inputNeuron), 
                           outputNeuron(outputNeuron) { }

        public: 

            /* the historical id if this */
            const uint64_t id;

            /* the input and output neuronal gene */
            const NeuronGene inputNeuron, outputNeuron;

            /* creats a new Gene with two given neuron ids */
            static ConnectionGene *New(const NeuronGene inputNeuron, 
                                       const NeuronGene outputNeuron,
                                       double weight = rand_range_d64(rand, -1.0, 1.0)) {
                
                pthread_mutex_lock(&mutex);
                const unsigned id = idCounter++;
                pthread_mutex_unlock(&mutex);

                return new ConnectionGene(inputNeuron, outputNeuron, weight, id);
            }
            
            /* returns the weight of this */
            double getWeight() const { return weight; }

            /* disables this gene */
            void disable() { enabled = false; }

            /* resets the waight of this to a random value */
            void mutate() { weight = rand_range_d64(rand, -1.0, 1.0); }

            /* compares to genes based on their 2D location */
            bool operator == (const ConnectionGene &other) const {
                return inputNeuron  == other.inputNeuron && 
                       outputNeuron == other.outputNeuron;
            }
    };

}

#endif /* __GENE_H__ */
