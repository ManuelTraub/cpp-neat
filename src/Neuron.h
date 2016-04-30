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

        public:

            /* returns the output value of this */
            double getValue() { return outputValue; }

            /* set the value for input neurons */
            void setValue(double value) { outputValue = value; }

            /* calculates the output value from the input values */
            void update() {
                
                double weightedInputs = 0.0;
                for (unsigned i = 0; i < inputs.size(); i++)
                    weightedInputs += weights[i] * inputs[i]->getValue();

                /* calculate output using simloid 
                 * using tanh as below is a fast aproximation for 1/(1 + e^-2x)
                 */
                outputValue = 0.5 * tanh(weightedInputs) + 0.5;
            }

            /* adds an input */
            void addInput(Neuron *input, double weight) {
                this->inputs.push_back(input);
                this->weights.push_back(weight);
            }

    };

};

#endif /* __NEURON_H__ */
