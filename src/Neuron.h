/**
 * Implementation of a singel neuron within a Neuronal Net
 */
#ifndef __NEURON_H__
#define __NEURON_H__

#include <vector>

using std::vector;

namespace NEAT {

    class Neuron {
        
        private:

            /* the input connections of this */
            vector<Neuron *> inputs;

            /* the weights for each input connection */
            vector<double> weights;

            /* the output value of this */
            double outputValue;

            /* temporary output value to store update result till all 
             * neurons of the same layer have calculated the new output value
             * (to handle recurent connections within the same layer)
             */
            double tempOutputValue;

        public:

            /* returns the inputs of this */
            vector<Neuron *> getInputs() const { return inputs; }

            /* returns the weights of this */
            vector<double> getWeights() const { return weights; }

            /* returns the output value of this */
            double getValue() const { return outputValue; }

            /* set the value for input neurons */
            void setValue(double value) { outputValue = value; }

            /* calculates the output value from the input values */
            void calculateUpdate() {
                
                double weightedInputs = 0.0;
                for (unsigned i = 0; i < inputs.size(); i++)
                    weightedInputs += weights[i] * inputs[i]->getValue();

                /* calculate output using a modified simloid
                 */
                tempOutputValue = 1.0/(1.0 + exp(-4.924273 * weightedInputs));
            }

            /* applays the output value */
            void applayUpdate() {
                outputValue = tempOutputValue;
            }

            /* adds an input */
            void addInput(Neuron *input, double weight) {
                this->inputs.push_back(input);
                this->weights.push_back(weight);
            }

    };

};

#endif /* __NEURON_H__ */
