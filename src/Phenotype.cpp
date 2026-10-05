#include "Phenotype.h"
#include <assert.h>
#include <set>
#include <unordered_map>
#include "../lib/simple_svg_1.0.0.hpp"
#include "utils/utils.h"
#include <IL/il.h>
#include <unistd.h>

using namespace svg;
using namespace NEAT;
using std::set;
using std::unordered_map;
using std::make_pair;

/* create a new Phenotype out of the given Genotype */
Phenotype::Phenotype(const Genotype &genome, Opts &opts, bool storeFirings) : 
                     opts(opts.freeze()), genome(genome), storeFirings(storeFirings) {

    unordered_map<uint64_t, NeuronGene> neuronGenes         = genome.getNeurons();
    unordered_map<uint64_t, ConnectionGene> connectionGenes = genome.getConnections();

    /* create a sorted list of layer ids from all 
     * neurons in order to calculate the number of layers 
     */
    set<uint64_t> layerIDs;

    for (const auto &e : neuronGenes)
        layerIDs.insert(e.second.getLayerID());

    /* map a layer index to each different xValue */
    unordered_map<uint64_t, unsigned> layerIndices;
    
    unsigned layerIndex = 0;
    for (auto &id: layerIDs) {
        layerIndices.insert({id, layerIndex});
        layerIndex++;

        /* create the desired number of layers */
        this->neuronLayers.push_back(vector<Neuron *>());
    }

    /* create all neurons and store them efficiently in a unordered_map */
    for (const auto &e : neuronGenes) {

        /* store neuron for efficient access */
        Neuron *const newNeuron = new Neuron(e.second.getGradient(), e.second.getFunction());
        neuronMap.insert({e.second.getID(), newNeuron});

        /* store the neuron in its desired layer */
        const unsigned layer = layerIndices[e.second.getLayerID()];
        if (e.second.getType() == NeuronType::BIAS)
            this->bias = newNeuron;
        else if (e.second.getType() == NeuronType::RAND_BIAS) {
            this->randomBiases.push_back(newNeuron);
            this->randomBiasGenerators.push_back(new_rand64_t());
        } else
            this->neuronLayers[layer].push_back(newNeuron);

#ifdef __DEBUG__
        const NeuronGene &neuron = e.second;

        if (layer == 0 && 
            neuron.getType() != NeuronType::INPUT && 
            neuron.getType() != NeuronType::BIAS  &&
            neuron.getType() != NeuronType::RAND_BIAS) {
            
            assertf(false, "invalid neuron in first layer: type(%u) id(%lu)\n", 
                    (unsigned) neuron.getType(), neuron.getID());
        }

        if (layer == this->neuronLayers.size() - 1 && neuron.getType() != NeuronType::OUTPUT) {
            assertf(false, "invalid neuron in last layer: type(%u) id(%lu)\n", 
                    (unsigned) neuron.getType(), neuron.getID());
        }

        if (layer > 0 && 
            layer < this->neuronLayers.size() - 1 && 
            neuron.getType() != NeuronType::HIDDEN) {

            assertf(false, "invalid neuron in hidden layer %u: type(%u) id(%lu)\n", layer,
                    (unsigned) neuron.getType(), neuron.getID());
        }
#endif /* __DEBUG__ */
    }

    /* set the bias to 1 */
    this->bias->setValue(1);

    /* connect the neurons */
    for (const auto &e : connectionGenes) {
        
        if (e.second.isEnabled()) {
            Neuron *const input  = neuronMap[e.second.getInput().getID()];
            Neuron *const output = neuronMap[e.second.getOutput().getID()];

            assert(input != NULL && output != NULL);
            output->addInput(input, e.second.getWeight());
        }
    }
}

/* resets the network and introduces random dynamics */
void Phenotype::initNet(unsigned cycles) {

    for (auto &layer: neuronLayers)
        for (auto &neuron: layer)
            neuron->reset();
    
    for (unsigned i = 0; i < cycles; i++) {
        if (opts.netActivationFunction() == ActivationFunction::SPIKING) 
            this->runSpiking(vector<double>(this->neuronLayers[0].size(), 0.0));

        this->runSinglePass(vector<double>(this->neuronLayers[0].size(), 0.0));
    }
}

/* calculate the output for a given input */
vector<double> Phenotype::run(vector<double> inputs) {

    if (opts.resetNetBeforRun())
        this->initNet();

    if (opts.netActivationFunction() == ActivationFunction::SPIKING) 
        return this->runSpiking(inputs);

    return this->runSinglePass(inputs);
}

/* calculate the output for a given input within a single pass trough the network */
vector<double> Phenotype::runSinglePass(vector<double> inputs) {

    assert(inputs.size() == neuronLayers[0].size() || inputs.empty());
    vector<double> outputs(this->neuronLayers.back().size());
    
    /* set the input neurons */
    for (unsigned i = 0; i < inputs.size(); i++)
        this->neuronLayers[0][i]->setValue(inputs[i]);

    /* set random biases */
    for (unsigned i = 0; i < randomBiases.size(); i++)
        this->randomBiases[i]->setValue(rand_range_d64(randomBiasGenerators[i], -1.0, 1.0));
    
    if (opts.layeredNetwork()) {

        /* calculate the output for each layer (except the input layers)
         * (calculate and applay in several steps to preserv recurent
         * connections within the same layer)
         */
        for (unsigned l = 1; l < neuronLayers.size(); l++) {

            for (unsigned n = 0; n < neuronLayers[l].size(); n++) 
                neuronLayers[l][n]->calculateWeightedInputs();

            for (unsigned n = 0; n < neuronLayers[l].size(); n++) 
                neuronLayers[l][n]->calculateOutput();
        }
    } else {

        /* clacluate the output for each neuron */
        for (unsigned l = 1; l < neuronLayers.size(); l++) 
            for (unsigned n = 0; n < neuronLayers[l].size(); n++) 
                neuronLayers[l][n]->calculateWeightedInputs();

        for (unsigned l = 1; l < neuronLayers.size(); l++)
            for (unsigned n = 0; n < neuronLayers[l].size(); n++) 
                neuronLayers[l][n]->calculateOutput();
        
        if (storeFirings) {
            vector<bool> curFirings;

            for (unsigned l = 1; l < neuronLayers.size(); l++)
                for (unsigned n = 0; n < neuronLayers[l].size(); n++) 
                    curFirings.push_back(neuronLayers[l][n]->getValue() > 0.9);

            this->firings.push_back(curFirings);
        }
    }

    /* collect the output values */
    for (unsigned i = 0; i < neuronLayers.back().size(); i++)
        outputs[i] = neuronLayers.back()[i]->getValue();

    return outputs;
}

/* calculate the output of a spiking neural network for a given input */
vector<double> Phenotype::runSpiking(vector<double> inputs) {

    assert(inputs.size() == neuronLayers[0].size() || inputs.empty());
    vector<double> outputs(this->neuronLayers.back().size(), -1.0);

    if (opts.spikeOutputEncoding() == Opts::SpikeCoding::FREQUENCY) {
        for (unsigned i = 0; i < opts.spikingNumSimulationStepsPerOutoput(); i++) {
            vector<double> cur = this->runSinglePass(inputs);
            for (unsigned n = 0; n < outputs.size(); n++)
                outputs[n] += cur[n];
        }

        for (unsigned n = 0; n < outputs.size(); n++) {
            outputs[n] /= opts.spikingNumSimulationStepsPerOutoput() / 2;
            outputs[n] -= 1;
        }

    } else if (opts.spikeOutputEncoding() == Opts::SpikeCoding::TIME) {

        for (unsigned i = 0, setted = 0; 
             setted < outputs.size() && i < opts.spikingMaxOutputSiumlationSteps(); 
             i++) {

            vector<double> cur = this->runSinglePass(inputs);

            double curValue = opts.spikingMaxOutputSiumlationSteps() - 2*i;
            curValue /= opts.spikingMaxOutputSiumlationSteps();

            for (unsigned n = 0; n < outputs.size(); n++) {
                if (outputs[n] < curValue && cur[n] > 0.5) {
                    outputs[n] = curValue;
                    setted++;
                }
            }
        }
    } else if (opts.spikeOutputEncoding() == Opts::SpikeCoding::TIME_WINDOW) {

        for (unsigned i = 0; i < opts.spikingMaxOutputSiumlationSteps(); i++) {

            vector<double> cur = this->runSinglePass(inputs);

            double curValue = opts.spikingMaxOutputSiumlationSteps() - 2*i;
            curValue /= opts.spikingMaxOutputSiumlationSteps();

            for (unsigned n = 0; n < outputs.size(); n++) {
                if (outputs[n] < curValue && cur[n] > 0.5) {
                    outputs[n] = curValue;
                }
            }
        }
    }

    return outputs;
}



/* returns the index of the output with tha higest value */
unsigned Phenotype::maxOutput(vector<double> inputs) {

    double maxOutput  = 0;
    unsigned maxIndex = (unsigned) -1;
    const vector<double> outputs = this->run(inputs);

    for (unsigned i = 0; i < outputs.size(); i++) {
        if (outputs[i] > maxOutput) {
            maxOutput = outputs[i];
            maxIndex  = i;
        }
    }

    return maxIndex;
}

/* calculaes the quadratic error for the given data set */
double Phenotype::calculateError(vector<vector<double>> inputs, vector<vector<double>> outputs) {

    assert(inputs.size() == outputs.size());
    double error = 0;

    for (unsigned k = 0; k < inputs.size(); k++) {

        assert(inputs[k].size() == neuronLayers[0].size());
        assert(outputs[k].size() == neuronLayers.back().size());

        const vector<double> curOutputs = this->run(inputs[k]);
        
        /* calculate the round error */
        for (unsigned i = 0; i < curOutputs.size(); i++) {
            error += (outputs[k][i] - curOutputs[i]) * (outputs[k][i] - curOutputs[i]);
            
            /* penalize unconnected outputs */
            if (neuronLayers.back()[i]->getInputs().size() == 0)
                error += 1;
        }
    }

    assert(!std::isnan(error));
    return error;
}

/* calculaes the maximum derivation for a single output from the training data */
double Phenotype::maxDerivation(vector<vector<double>> inputs, vector<vector<double>> outputs) {

    assert(inputs.size() == outputs.size());
    double max = 0;

    for (unsigned k = 0; k < inputs.size(); k++) {

        assert(inputs[k].size() == neuronLayers[0].size());
        assert(outputs[k].size() == neuronLayers.back().size());

        const vector<double> curOutputs = this->run(inputs[k]);
        
        /* calculate the round error */
        for (unsigned i = 0; i < curOutputs.size(); i++) {
            double derivation = absDiff(outputs[k][i], curOutputs[i]);

            /* penalize unconnected outputs */
            if (neuronLayers.back()[i]->getInputs().size() == 0)
                derivation += 1;
            
            if (derivation > max)
                max = derivation;
        }
    }
    return max;
}

/* export this as an svg image */
void Phenotype::saveAsSVG(string fname) {

    /* calculate the maximum neurons per layer */
    unsigned maxNeurons = 0;
    for (auto &layer : neuronLayers)
        if (layer.size() > maxNeurons)
            maxNeurons = layer.size();

    const unsigned height         = 1000;
    const unsigned neuronDiameter = (maxNeurons > 1) ? (height / ((maxNeurons - 1) * 4)) : height / 4;
    const unsigned connectionSize = (neuronDiameter / 7 > 1) ? neuronDiameter / 7 : 1;
    const double   circleStroke   = connectionSize / 2.0;
    const unsigned nLayers        = neuronLayers.size();
    const unsigned width          = neuronDiameter * 4 * (nLayers - 1) + neuronDiameter + 50;

    Dimensions dimensions(width, height);
    Document doc(fname, Layout(dimensions, Layout::BottomLeft));

    /* create the neurons and store them efficiently */
    unordered_map<Neuron *, Point> points;
    unordered_map<Neuron *, Color> colorsRecurent;
    unordered_map<Neuron *, Color> colorsBias;

    /* calculate the points for all neurons */
    for (unsigned l = 0; l < nLayers; l++) {

        const unsigned nNeurons = neuronLayers[l].size();
        const unsigned stepSize = (nNeurons > 1) ? (height - (50 + neuronDiameter)) / (nNeurons - 1) : 1;
        for (unsigned n = 0; n < nNeurons; n++) {
        
            const Point point(25 + neuronDiameter / 2 + l * neuronDiameter * 4, 
                              ((nNeurons > 1) ? 25 + neuronDiameter / 2 + stepSize * n : height / 2));

            points.insert(make_pair(neuronLayers[l][n], point));
            colorsRecurent.insert(make_pair(neuronLayers[l][n], Color::Black));
            colorsBias.insert(make_pair(neuronLayers[l][n], Color::Blue));
        }
    }

    /* paint all connections */
    for (auto &layer : neuronLayers) {
        for (auto &neuron : layer) {
             vector<Neuron *> inputs = neuron->getInputs();
             vector<double> weights  = neuron->getWeights();

             for (unsigned i = 0; i < inputs.size(); i++) {

                const unsigned green = (weights[i] < 0) ? (255.0 * (8 + weights[i])) / 8 : 255;
                const unsigned red   = (weights[i] > 0) ? (255.0 * (8 - weights[i])) / 8 : 255;
                
                 /* color node fill color if bias connection */
                 if (inputs[i] == bias) {
                     colorsBias[neuron] = Color(red, green, 0);

                 /* color node border if self recurent */
                 } else if (points[inputs[i]].x == points[neuron].x &&
                     points[inputs[i]].y == points[neuron].y) {

                     colorsRecurent[neuron] = Color(red, green, 0);

                 /* dash recurent connections */
                 } else if (points[inputs[i]].x > points[neuron].x) {

                    doc << (Polyline(Stroke(connectionSize*2, Color(red, green, 0), Dashed(100,100))) << 
                            points[inputs[i]] << points[neuron]);

                 /* draw normal line for normal connections */
                 } else {
                
                    doc << (Polyline(Stroke(connectionSize, Color(red, green, 0))) << 
                            points[inputs[i]] << points[neuron]);
                 }
             }
        }
    }

    /* paint all neurons */
    for (auto &e : points) {
        doc << Circle(e.second, 
                      neuronDiameter, 
                      colorsBias[e.first], 
                      Stroke(circleStroke, colorsRecurent[e.first]));

        string neuronFunction = "Error";
        if (e.first->getFunction() == ActivationFunction::SIGMOID)
            neuronFunction = "sigmoid";
        else if (e.first->getFunction() == ActivationFunction::TANH)
            neuronFunction = "tanh";
        else if (e.first->getFunction() == ActivationFunction::RELU)
            neuronFunction = "relu";
        else if (e.first->getFunction() == ActivationFunction::SINE)
            neuronFunction = "sine";
        else if (e.first->getFunction() == ActivationFunction::LOG)
            neuronFunction = "log";
        else if (e.first->getFunction() == ActivationFunction::SQRT)
            neuronFunction = "sqrt";
        else if (e.first->getFunction() == ActivationFunction::ABS)
            neuronFunction = "abs";
        else if (e.first->getFunction() == ActivationFunction::POW)
            neuronFunction = "pow";
        else if (e.first->getFunction() == ActivationFunction::EXP)
            neuronFunction = "exp";
        else if (e.first->getFunction() == ActivationFunction::ADD)
            neuronFunction = "add";
        else if (e.first->getFunction() == ActivationFunction::MULT)
            neuronFunction = "mult";
        else if (e.first->getFunction() == ActivationFunction::GAUSSIAN)
            neuronFunction = "gaus";
        else if (e.first->getFunction() == ActivationFunction::SPIKING)
            neuronFunction = "spiking";

        Point p(e.second.x - 50, e.second.y);
        if (opts.randomActivationFunction()) 
            doc << Text(p, neuronFunction, Color::Black, Font(30, "Verdana"));
    }

    doc.save();
}

/* create an image from a frigin pattern */
void Phenotype::createFiringPattern(std::string fileName) {

    if (!endsWith(fileName, ".png"))
        fileName += ".png";

    const unsigned numSimulationSteps = this->firings.size();
    const unsigned numNeurons         = this->firings[0].size();

    unlink(fileName.c_str());
    uint8_t *pixels = (uint8_t *) calloc(numNeurons * numSimulationSteps * 3, sizeof(uint8_t));

    for (unsigned t = 0; t < numSimulationSteps; t++) {
        for (unsigned n = 0; n < numNeurons; n++) {
            if (firings[t][n]) {
                pixels[numNeurons * 3 * t + n * 3 + 0] = 0;
                pixels[numNeurons * 3 * t + n * 3 + 1] = 255;
                pixels[numNeurons * 3 * t + n * 3 + 2] = 0;
            }
        }
    }

    ilInit();
    ilTexImage(numNeurons, numSimulationSteps, 0, 3, IL_RGB, IL_UNSIGNED_BYTE, pixels);
    ilSaveImage(fileName.c_str());
}

/* prints the weights of this */
void Phenotype::printWeights() {
    for (auto &layer: neuronLayers) {
        for (auto &neuron: layer) {
            for (auto &w: neuron->getWeights())
                printf("%F ", w);

            printf("\n");
        }
        printf("\n");
    }
}

/* destructor */
Phenotype::~Phenotype() {

    for (auto &layer : neuronLayers) 
        for (auto &neuron : layer) 
            delete neuron;

    delete this->bias;

    for (unsigned i = 0; i < randomBiases.size(); i++) {
        delete randomBiases[i];
        free(randomBiasGenerators[i]);
    }
}
