/**
 * Abstract Gene representation within NEAT
 * each Gene has an historical id to track back
 * its origin
 */
#ifndef __GENE_H__
#define __GENE_H__

#include <inttypes.h>

namespace NEAT {

    /**
     * A Neuron Gene representing a neuron within a neural network
     */
    class NeuronGene {
        
        private:
            
            /* the id of this neuron 
             * Neurons are enumerated by each organism,
             * making it posible track similar ConnectionGene creation
             * */
            unsigned id;

            /* the layer of this neuron (0 is first hidden layer) */
            unsigned layer;

            /* the cummulated input inputs */
            double cummulatedInput;

            /* the output value */
            double outputValue;

        public:
            
            /* creats a new neuron within the given layer, and with the given id */
            NeuronGene(unsigned id, unsigned layer) {
                this->id    = id;
                this->layer = layer;
            }

            /* clears the cummulated inputs to start recaluclation
             * of the output value of this */
            void clearInputs() {
                cummulatedInput = 0.0;
            }

            /* adds the value of ine input tho the cummulated input value of this 
             * in order to generate the output value of this */
            void addInput(double value) {
                cummulatedInput += value;
            }

            /* calculates the output value once all input vallues are cummulated */
            void calculateOutput() {

                /* calculate output using simloid 
                 * using tanh as below is a good aproximation for 1/(1 + e^-2x)
                 */
                outputValue = 0.5 * tanh(cummulatedInput) + 0.5;
            }

            /* set the outputValue for input neurons */
            void setOutput(double outputValue) {
                this->outputValue = outputValue;
            }

            /* returns the output value of this */
            const double getOutput() { 
                return outputValue;
            }
    };

    uint64_t NeuronGene::idCounter = 0;

    /**
     * A Gene representing a connection between two neurons
     */
    class ConnectionGene {
        
        private:

            /* the global id counter */
            static uint64_t idCounter; 

            /* the historical id if this */
            const uint64_t id;
            
            /* bool indicates if this gene is expressed within the phenotype */
            bool enabled;

            /* the neuron number of the input and output neuron of this Gene 
             * ids = 0 ==> bias neuron
             * ids <= inputNeurons ==> input neuron
             * inputNeurons <= ids < outputNeurons ==> output neuron
             * outputNeurons <= ids ==> hidden neuron
             */
            const unsigned inputNeuron, outputNeuron;

            /**
             * the layers for neurons of this connection
             */
            const unsigned inputLayer, outputLayer;

            /* the weight of this connection */
            double weight;

        public: 

            /* creats a new Gene and initializes its id */
            
            /* returns the historical id of this */
            uint64_t getID() { return id; }

            /* returns the inputNeurons of this */
            unsigned getInputNeuron() { return inputNeuron; }

            /* returns the output neuron of this */
            unsigned getOutputNeuron() { return outputNeuron; }

            /* returns the input layers of this */
            unsigned getInputLayer() { return inputLayer; }

            /* returns the output layer of this */
            unsigned getOutputLayer() { return outputLayer; }

            /* returns the weight of this */
            double getWeight() { return weight; }

    };

    uint64_t Gene::idCounter = 0;

}

#endif /* __GENE_H__ */
