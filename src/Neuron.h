/**
 * Implementation of a singel neuron within a Neuronal Net
 */
#ifndef __NEURON_H__
#define __NEURON_H__

#include "Opts.h"
#include <vector>

using std::vector;

namespace NEAT {

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

                /* calculate the output value using a sine activation */
                void calculateSineOutput() {
                    outputValue = sin(gradient * weightedInputs);
                }

                /* calculate the output value using a log activation */
                void calculateLogOutput() {
                    if (gradient * weightedInputs < 0.36787944117144233)
                        outputValue = -1;
                    else 
                        outputValue = log(gradient * weightedInputs);
                }

                /* calculate the output value using a sqrt activation */
                void calculateSqrtOutput() {
                    if (gradient * weightedInputs < 0)
                        outputValue = 0;
                    else 
                        outputValue = sqrt(gradient * weightedInputs);
                }

                /* calculate the output value using a abs activation */
                void calculateAbsOutput() {
                    outputValue = std::abs(gradient * weightedInputs);
                }

                /* calculate the output value using a pow 2 activation */
                void calculatePow2Output() {
                    outputValue = pow(gradient * weightedInputs, 2);
                }

                /* calculate the output value using a exp activation */
                void calculateExpOutput() {
                    if (gradient * weightedInputs > 0)
                        outputValue = 1;
                    else 
                        outputValue = exp(gradient * weightedInputs);
                }

                /* calculate the output value using an identity activation */
                void calculateIdentityOutput() {
                    outputValue = gradient * weightedInputs;
                }

                /* calculate the output value using an identity activation */
                void calculateGaussianOutput() {
                    outputValue = 1.0 / (gradient * 2.5066282746310002);
                    outputValue *= exp(pow(weightedInputs, 2) / (-2 * pow(gradient, 2)));
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

                    if (function == ActivationFunction::MULT) {

                        weightedInputs = 1.0;
                        for (unsigned i = 0; i < inputs.size(); i++)
                            weightedInputs *= weights[i] * inputs[i]->getValue();

                    } else {
                    
                        weightedInputs = 0.0;
                        for (unsigned i = 0; i < inputs.size(); i++)
                            weightedInputs += weights[i] * inputs[i]->getValue();
                    }
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
                    else if (function == ActivationFunction::LOG)
                        this->calculateLogOutput();
                    else if (function == ActivationFunction::SQRT)
                        this->calculateSqrtOutput();
                    else if (function == ActivationFunction::ABS)
                        this->calculateAbsOutput();
                    else if (function == ActivationFunction::POW)
                        this->calculatePow2Output();
                    else if (function == ActivationFunction::EXP)
                        this->calculateExpOutput();
                    else if (function == ActivationFunction::ADD)
                        this->calculateIdentityOutput();
                    else if (function == ActivationFunction::MULT)
                        this->calculateIdentityOutput();
                    else if (function == ActivationFunction::GAUSSIAN)
                        this->calculateGaussianOutput();
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
                    periodicValue(0.0) { }

                /* resets this to zero */
                void reset() {
                    this->outputValue    = 0.0;
                    this->weightedInputs = 0.0;
                }

                void setWeightedInputs(double inputs) { weightedInputs = inputs; }

        };
};

#endif /* __NEURON_H__ */
