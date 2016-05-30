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
#include "HexTree.h"

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

            /* the neuron type of this */
            NeuronType type;

            /* the x raster index */
            uint64_t xRaster;

            /* the y raster index */
            uint64_t yRaster;

        public:

            /* returns the type of this */
            NeuronType getType() const { return type; }

            /* returns the xRaster of this */
            uint64_t getX() const { return xRaster; }

            /* returns the yRaster of this */
            uint64_t getY() const { return yRaster; }

            /* creats a new NeuronGeneID */
            NeuronGene(
                const NeuronType type,
                const uint64_t xRaster, 
                const uint64_t yRaster
            ) : type(type), xRaster(xRaster), yRaster(yRaster) { }

            /* returns wether a new node can be created between the two given nodes */
            bool canBeInserted(const NeuronGene &gene1, const NeuronGene &gene2) {
                return absDiff(gene1.xRaster, gene2.xRaster) > 1 ||
                       absDiff(gene1.yRaster, gene2.yRaster) > 1;
            }

            /* create a new node between two nodes */
            NeuronGene(
                const NeuronType type,
                const NeuronGene &gene1, 
                const NeuronGene &gene2
            ) : type(type),
                xRaster(middle(gene1.xRaster, gene2.xRaster)), 
                yRaster(middle(gene1.yRaster, gene2.yRaster)) {

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

            /* the input and output neuronal gene */
            NeuronGene inputNeuron, outputNeuron;

            /* the weight of this connection */
            double weight;

            /* bool indicates if this gene is expressed within the phenotype */
            bool enabled;

            /* the historical id if this */
            uint64_t id;

        public: 

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
            }

            /* returns the historical id of this */
            uint64_t getID() const { return id; }

            /* returns the inputNeuron of this */
            NeuronGene getInput() const { return inputNeuron; }

            /* returns the outputNeuron of this */
            NeuronGene getOutput() const { return outputNeuron; }
            
            /* returns the weight of this */
            double getWeight() const { return weight; }

            /* set the weight */
            void setWeight(const double weight) {
                assert(weight >= -1.0 && weight <= 1.0);
                this->weight = weight;
            }

            /* disables this gene */
            void disable() { assert(enabled); enabled = false; }

            /* returns if this is enabled */
            bool isEnabled() const { return enabled; }

            /* resets the waight of this to a random value */
            void mutate() { weight = rand_range_d64(rand, -1.0, 1.0); }

            /* compares to genes based on their 2D location */
            bool operator == (const ConnectionGene &other) const {
                return inputNeuron  == other.inputNeuron && 
                       outputNeuron == other.outputNeuron;
            }

            /* returns a new NeuronGene spliting this */
            NeuronGene *split() const {
                return new NeuronGene(NeuronType::HIDDEN, inputNeuron, outputNeuron);
            }
    };

    class NeuronTree : public QuadTree<NeuronGene> {
        
        public :
            
            /* insert a neuron into this if its not already exist */
            NeuronGene *insert(NeuronGene *const gene) {
                return QuadTree<NeuronGene>::insert(gene, gene->getX(), gene->getY());
            }
    };

    class ConnectionTree : public HexTree<ConnectionGene> {

        public: 

            /* insert a neuron into this if its not already exist */
            ConnectionGene *insert(ConnectionGene *const gene) {
                return HexTree<ConnectionGene>::insert(gene, 
                                                       gene->getInput().getX(), 
                                                       gene->getInput().getY(), 
                                                       gene->getOutput().getX(),
                                                       gene->getOutput().getY());
            }
    };

}

#endif /* __GENE_H__ */
