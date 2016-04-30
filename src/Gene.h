/**
 * Abstract Gene representation within NEAT
 * each Gene has an historical id to track back
 * its origin
 */
#ifndef __GENE_H__
#define __GENE_H__

#include <inttypes.h>
#include <pthread.h>
#include "utils.h"

namespace NEAT {

    /* the different types of neurons (input output or hidden) */
    enum class NeuronType { INPUT, OUTPUT, HIDDEN };

    /**
     * A Neuron Gene representing a neuron within a neural network
     */
    struct NeuronGene {

        /* the neuron type of this */
        const NeuronType type;
        
        /* the id of this neuron 
         * Neurons are enumerated by each organism,
         * making it posible track similar ConnectionGene creation
         * */
        const unsigned id;

        /* the layer of this neuron (0 is first hidden layer) */
        const unsigned layer;
        
        /* creats a new neuron within the given layer, and with the given id */
        NeuronGene(const NeuronType type, 
                   const unsigned id, 
                   const unsigned layer) :
                   type(type), id(id), layer(layer) { }

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
            static rand128_t *rand;

            /* bool indicates if this gene is expressed within the phenotype */
            bool enabled;

            /* the weight of this connection */
            double weight;

            /* private constructor that asigns the historical id */
            ConnectionGene(const unsigned inputNeuron, 
                           const unsigned outputNeuron,
                           double weight,
                           uint64_t id) :
                           weight(weight),
                           id(id),
                           inputNeuron(inputNeuron), 
                           outputNeuron(outputNeuron) { }

        public: 

            /* the historical id if this */
            const uint64_t id;

            /* the neuron number of the input and output neuron of this Gene 
             * ids = 0 ==> bias neuron
             * ids <= inputNeurons ==> input neuron
             * inputNeurons <= ids < outputNeurons ==> output neuron
             * outputNeurons <= ids ==> hidden neuron
             */
            const unsigned inputNeuron, outputNeuron;

            /* creats a new Gene with two given neuron ids */
            static ConnectionGene *New(const unsigned inputNeuron, 
                                const unsigned outputNeuron,
                                double weight = rand_range_d128(rand, -1.0, 1.0)) {
                
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
            void mutate() { weight = rand_range_d128(rand, -1.0, 1.0); }

    };

}

#endif /* __GENE_H__ */
