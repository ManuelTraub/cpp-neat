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

                /* the ActivationFunction of this */
                const ActivationFunction function;

                /* the gradient of how fast this neuron switches from min to max */
                const double gradient;

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
                
                /* temporary value for periodic signals */
                double periodicValue;

                /* the membran potential for a spiking neuron */
                double v;

                /* membran recovery variable for a spiking neuron */
                double u;

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

                /* clalculates a sine signal with perid length dependet on the inputs */
                void calculateSineOutput() {
                    periodicValue += PI/(1.0 + exp(-gradient * weightedInputs));
                    outputValue = sin(periodicValue);
                }

                /* calculate the output using a spiking neuron */
                void calculateSpikingOutput() {
                    v += 0.5 * (0.04 * v * v + 5 * v + 140 - u + weightedInputs + gradient);
                    v += 0.5 * (0.04 * v * v + 5 * v + 140 - u + weightedInputs + gradient);
                    u += 0.02 * (0.2 * v - u);
                    outputValue = 0.0;

                    if (v >= 30) {
                        u += 8;
                        v  = -65;
                        outputValue = 1.0;
                    }
                }

            public:

                /* returns the inputs of this */
                vector<Neuron *> getInputs() const { return inputs; }

                /* returns the weights of this */
                vector<double> getWeights() const { return weights; }

                /* returns the gradient of this */
                double getGradient() const { return gradient; }

                /* returns the activation function of this */
                ActivationFunction getFunction() const { return function; }

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

                /* calculates the output of this */
                void calculateOutput() {
                    
                    if (function == ActivationFunction::SIGMOID)
                        this->calculateSigmoidOutput();
                    else if (function == ActivationFunction::TANH)
                        this->calculateTanhOutput();
                    else if (function == ActivationFunction::RELU)
                        this->calculateReLUOutput();
                    else if (function == ActivationFunction::SINE)
                        this->calculateSineOutput();
                    else if (function == ActivationFunction::SPIKING)
                        this->calculateSpikingOutput();
                    else
                        assert(false);
                }

                /* adds an input */
                void addInput(Neuron *input, double weight) {
                    this->inputs.push_back(input);
                    this->weights.push_back(weight);
                }

                /* creats a new Neuron */
                Neuron(const double gradient, const ActivationFunction function) :
                    function(function),
                    gradient(gradient),
                    outputValue(0.0), 
                    weightedInputs(0.0), 
                    periodicValue(0.0),
                    v(-65),
                    u(8) { }

                /* resets this to zero */
                void reset() {
                    this->outputValue    = 0.0;
                    this->weightedInputs = 0.0;
                    this->v = -65;
                    this->u = 8;
                }

        };
    }
};

#endif /* __NEURON_H__ */
