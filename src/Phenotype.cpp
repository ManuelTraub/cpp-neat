#include "Phenotype.h"
#include <assert.h>
#include <set>
#include <unordered_map>
#include "../lib/simple_svg_1.0.0.hpp"

using namespace svg;
using namespace NEAT;
using std::set;
using std::unordered_map;
using std::make_pair;

/* create a new Phenotype out of the given Genotype */
Phenotype::Phenotype(Genotype *genome) : genome(genome) {

    vector<NeuronGene *> neuronGenes         = genome->getNeurons();
    vector<ConnectionGene *> connectionGenes = genome->getConnections();

    /* create a sorted list of the x values from all 
     * neurons in order to calculate the number of layers 
     */
    set<uint64_t> xValues;

    for (auto &neuron : neuronGenes)
        xValues.insert(neuron->getX());

    /* map a layer index to each different xValue */
    unordered_map<uint64_t, unsigned> layerIndices;
    
    unsigned layerIndex = 0;
    for (auto &value : xValues) {
        layerIndices.insert(make_pair(value, layerIndex));
        layerIndex++;

        /* create the desired number of layers */
        this->neuronLayers.push_back(vector<Neuron *>());
    }

    /* create all neurons and store them efficiently in a QuadTree */
    QuadTree<Neuron> neuronTree;

    for (auto &neuron : neuronGenes) {

        /* store neuron for efficient access */
        Neuron *const newNeuron = new Neuron();
        neuronTree.insert(newNeuron, neuron->getX(), neuron->getY());

        /* store the neuron in its desired layer */
        const unsigned layer = layerIndices[neuron->getX()];
        if (neuron->getType() == NeuronType::BIAS)
            this->bias = newNeuron;
        else
            this->neuronLayers[layer].push_back(newNeuron);

#ifdef __DEBUG__
        if (layer == 0 && 
            neuron->getType() != NeuronType::INPUT && 
            neuron->getType() != NeuronType::BIAS) {
            
            assertf(false, "invalid neuron in first layer: type(%u) (%lu, %lu)\n", 
                    (unsigned) neuron->getType(), neuron->getX(), neuron->getY());
        }

        if (layer == this->neuronLayers.size() - 1 && neuron->getType() != NeuronType::OUTPUT) {
            assertf(false, "invalid neuron in last layer: type(%u) (%lu, %lu)\n", 
                    (unsigned) neuron->getType(), neuron->getX(), neuron->getY());
        }

        if (layer > 0 && 
            layer < this->neuronLayers.size() - 1 && 
            neuron->getType() != NeuronType::HIDDEN) {

            assertf(false, "invalid neuron in hidden layer %u: type(%u) (%lu, %lu)\n", layer,
                    (unsigned) neuron->getType(), neuron->getX(), neuron->getY());
        }
#endif /* __DEBUG__ */
    }

    /* set the bias to 1 */
    bias->setValue(1);

    /* connect the neurons */
    for (auto &connection : connectionGenes) {
        
        Neuron *const input  = neuronTree.get(connection->getInput().getX(), 
                                              connection->getInput().getY());
        Neuron *const output = neuronTree.get(connection->getOutput().getX(), 
                                              connection->getOutput().getY());

        assert(input != NULL && output != NULL);
        output->addInput(input, connection->getWeight());
    }
}

/* calculate the output for a given input */
vector<double> Phenotype::run(vector<double> inputs) {

    assert(inputs.size() == neuronLayers[0].size());
    vector<double> outputs(this->neuronLayers.back().size());
    
    /* set the input neurons */
    for (unsigned i = 0; i < inputs.size(); i++)
        this->neuronLayers[0][i]->setValue(inputs[i]);
    
    /* calculate the output for each layer (except the input layers)
     * (calculate and applay in several steps to preserv recurent
     * connections within the same layer)
     */
    for (unsigned l = 1; l < neuronLayers.size(); l++) {

        for (unsigned n = 0; n < neuronLayers[l].size(); n++) 
            neuronLayers[l][n]->calculateUpdate();

        for (unsigned n = 0; n < neuronLayers[l].size(); n++) 
            neuronLayers[l][n]->applayUpdate();
    }

    /* collect the output values */
    for (unsigned i = 0; i < neuronLayers.back().size(); i++)
        outputs[i] = neuronLayers.back()[i]->getValue();

    return outputs;
}

/* export this as an svg image */
void Phenotype::saveAsSVG(string fname) {

    /* calculate the maximum neurons per layer */
    unsigned maxNeurons = 0;
    for (auto &layer : neuronLayers)
        if (layer.size() > maxNeurons)
            maxNeurons = layer.size();

    const unsigned height         = 1000;
    const unsigned neuronDiameter = height / ((maxNeurons - 1) * 4);
    const unsigned connectionSize = (neuronDiameter / 7 > 1) ? neuronDiameter / 7 : 1;
    const double   circleStroke   = connectionSize / 2.0;
    const unsigned nLayers        = neuronLayers.size();
    const unsigned width          = neuronDiameter * 4 * (nLayers - 1) + neuronDiameter + 50;

    Dimensions dimensions(width, height);
    Document doc(fname, Layout(dimensions, Layout::BottomLeft));

    /* store the neurons and store them efficiently in a QuadTree */
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

                const unsigned green = (weights[i] < 0) ? 255.0 * (1 + weights[i]) : 255;
                const unsigned red   = (weights[i] > 0) ? 255.0 * (1 - weights[i]) : 255;
                
                 /* color node border if self recurent */
                 if (points[inputs[i]].x == points[neuron].x &&
                     points[inputs[i]].y == points[neuron].y) {

                     colorsRecurent[neuron] = Color(red, green, 0);

                 /* color node fill color if bias connection */
                 } else if (inputs[i] == bias) {
                     colorsBias[neuron] = Color(red, green, 0);
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
    }

    doc.save();
}

/* destructor */
Phenotype::~Phenotype() {

    for (auto &layer : neuronLayers) 
        for (auto &neuron : layer) 
            delete neuron;
}
