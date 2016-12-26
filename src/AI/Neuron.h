/**
 * Implementation of a singel neuron within a Neuronal Net
 */
#ifndef __NEURON_H__
#define __NEURON_H__

#include "Opts.h"
#include <vector>

using std::vector;

namespace NEAVC {

    namespace AI {

        class Neuron {
            
            private:

                /* the input connections of this */
                vector<Neuron *> inputs;

                /* the weights for each input connection */
                vector<double> weights;

                /* the output value of this */
                double outputValue;

                /* temporary value to store weighted inputs bevor updating the output value
                 * (to handle recurent connections within the same layer)
                 */
                double weightedInputs;

                /* the gradient of how fast this neuron switches from min to max */
                const double gradient;

            public:

                /* returns the inputs of this */
                vector<Neuron *> getInputs() const { return inputs; }

                /* returns the weights of this */
                vector<double> getWeights() const { return weights; }

                /* returns the output value of this */
                double getValue() const { return outputValue; }

                /* set the value for input neurons */
                void setValue(double value) { outputValue = value; }

                /* calculates the weighted input sum */
                void calculateWeightedInputs() {
                    
                    weightedInputs = 0.0;
                    for (unsigned i = 0; i < inputs.size(); i++)
                        weightedInputs += weights[i] * inputs[i]->getValue();
                }

                /* calculate the output value using a sigmoid activation */
                void calculateSigmoidOutput() {
                    outputValue = 1.0/(1.0 + exp(-gradient * weightedInputs));
                }

                /* calculate the output value using a tanh activation */
                void calculateTanhOutput() {
                    outputValue = tanh(gradient * weightedInputs);
                }

                /* calculate the output value using a rectangular liner unit activation */
                void calculateReLUOutput() {
                    outputValue = gradient * weightedInputs;
                    if (outputValue < 0) outputValue = 0;
                }

                /* adds an input */
                void addInput(Neuron *input, double weight) {
                    this->inputs.push_back(input);
                    this->weights.push_back(weight);
                }

                /* creats a new Neuron */
                Neuron(double gradient) : 
                    outputValue(0.0), 
                    weightedInputs(0.0), 
                    gradient(gradient) { }

                /* resets this to zero */
                void reset() {
                    this->outputValue    = 0.0;
                    this->weightedInputs = 0.0;
                }

        };
    }
};

#endif /* __NEURON_H__ */
