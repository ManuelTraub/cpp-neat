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
#include "GenerationCounter.h"

namespace NEAT {

    /* the different types of neurons (input output or hidden) */
    enum class NeuronType { BIAS = 0, INPUT = 1, HIDDEN = 2, OUTPUT = 3 };

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

            /* the generation this was invented or mutated */
            uint64_t generation;

        public:
            
            /* creates a new Gene */
            Gene(bool dummy = true) : generation(GenerationCounter::get()) {

                if (dummy) {
                    this->id         = UINT64_MAX;
                    this->generation = UINT64_MAX;
                } else {
                    pthread_mutex_lock(&mutex);
                    this->id = idCounter++;
                    pthread_mutex_unlock(&mutex);

                    assert(id != UINT64_MAX);
                    assert(generation <= GenerationCounter::get());
                }
            }

            /* returns whether this is a dummy gene */
            bool isDummy() const { return id == UINT64_MAX; }

            /* returns the xRaster of this */
            uint64_t getID() const { 
                assert(!isDummy());
                return id; 
            }

            /* returns the generation number this was invented or mutated */
            uint64_t getGeneration() const { 
                assert(!isDummy());
                assert(generation <= GenerationCounter::get());
                return generation; 
            }
    };

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

            /* the layer id of this (greater means nearer to the output layer) */
            uint64_t layerID;

        public:

            /* returns the type of this */
            NeuronType getType() const { assert(!isDummy()); return type; }

            /* returns the layer if of this */
            uint64_t getLayerID() const { assert(!isDummy()); return layerID; }

            /* creats a new NeuronGeneID */
            NeuronGene(const NeuronType type, const uint64_t layerID) : 
                       Gene(false), type(type), layerID(layerID) { 

                assert(type == NeuronType::INPUT  || 
                       type == NeuronType::OUTPUT ||
                       type == NeuronType::BIAS);
            }

            /* returns wether a new node can be created between the two given nodes */
            static bool canBeInserted(const NeuronGene &gene1, const NeuronGene &gene2);

            /* create a new node between two nodes */
            NeuronGene(
                const NeuronGene &gene1, 
                const NeuronGene &gene2
            ) : Gene(false),
                type(NeuronType::HIDDEN),
                layerID(middle(gene1.layerID, gene2.layerID)) {

                assert(canBeInserted(gene1, gene2));
            }

            /* compare operator */
            bool operator == (const NeuronGene &other) const {
                assert(!isDummy() && !other.isDummy());
                assert((getID() == other.getID() && layerID == other.layerID) ||
                       getID() != other.getID());

                return getID() == other.getID();
            }

            /* generate a dummy placeholder gene */
            NeuronGene() : type(NeuronType::HIDDEN), layerID(UINT64_MAX) { }
    };

    /**
     * A Gene representing a connection between two neurons
     */
    class ConnectionGene : public Gene {
        
        private:

            /* global random value to mutate this */
            static rand64_t *rand;

            /* the input and output neuronal gene */
            NeuronGene inputNeuron, outputNeuron;

            /* the weight of this connection */
            double weight;

            /* bool indicates if this gene is expressed within the phenotype */
            bool enabled;

        public: 

            /* generate a dummy placeholder gene */
            ConnectionGene() : weight(0), enabled(false) { }

            /* creats a new Gene with two given neuron ids */
            ConnectionGene(const NeuronGene inputNeuron, 
                           const NeuronGene outputNeuron,
                           double weight = rand_range_d64(rand, -1.0, 1.0)) : 
                           Gene(false),
                           inputNeuron(inputNeuron),
                           outputNeuron(outputNeuron),
                           weight(weight),
                           enabled(true) { }

            /* returns the inputNeuron of this */
            NeuronGene getInput() const { assert(!isDummy()); return inputNeuron; }

            /* returns the outputNeuron of this */
            NeuronGene getOutput() const { assert(!isDummy()); return outputNeuron; }
            
            /* returns the weight of this */
            double getWeight() const { assert(!isDummy()); return weight; }

            /* set the weight */
            void setWeight(const double weight) {
                assert(weight >= -1.0 && weight <= 1.0);
                assert(!isDummy());
                generation   = GenerationCounter::get();
                this->weight = weight;
            }

            /* disables this gene */
            void disable() { 
                assert(!isDummy() && enabled); 
                enabled    = false; 
                generation = GenerationCounter::get();
            }

            /* returns if this is enabled */
            bool isEnabled() const { assert(!isDummy()); return enabled; }

            /* resets the waight of this to a random value */
            void mutate() { 
                assert(!isDummy()); 
                weight = rand_range_d64(rand, -1.0, 1.0); 
                generation = GenerationCounter::get();
            }

            /* compares to genes based on their 2D location */
            bool operator == (const ConnectionGene &other) const {
                assert(!isDummy() && !other.isDummy());
                return inputNeuron  == other.inputNeuron && 
                       outputNeuron == other.outputNeuron;
            }

            /* returns thrue if the NeuronGene can be splitted */
            bool canBeSplitted() const {
                assert(!isDummy());
                return NeuronGene::canBeInserted(inputNeuron, outputNeuron);
            }

            /* returns a new NeuronGene spliting this */
            NeuronGene split() const {
                assert(!isDummy());
                return NeuronGene(inputNeuron, outputNeuron);
            }
    };
}

#endif /* __GENE_H__ */
