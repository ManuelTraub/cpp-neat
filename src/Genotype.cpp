#include "Genotype.h"

using namespace NEAT;
using std::make_pair;

/* global random value to mutate this */
rand128_t *Genotype::rand = new_rand128_t(time(NULL));

/* mutate this by inserting a new hidden neuron 
 * (returns the new created connections) */
pair<ConnectionGene *, ConnectionGene *> Genotype::mutateNeuron() {

    const unsigned index = rand128(rand) % connections.size();
    ConnectionGene *mutatingGene = connections[index];
    const unsigned newLayer = neurons[mutatingGene->inputNeuron]->layer + 1;

    /* increas the number of layers if neccessary */
    if (newLayer + 2 >= this->nLayers)
        this->nLayers++;

    /* to insert a NeuronGene without interfering to much with this
     * split an allready existing connecting into two parts
     * set the weight of the first part to 1 and the weight of the
     * second part to the weight of the splitted connection
     */
    neurons.push_back(new NeuronGene(NeuronType::HIDDEN, neurons.size(), newLayer));

    /* disable the old connection */
    mutatingGene->disable();

    /* create the two new genes */
    connections.push_back(ConnectionGene::New(mutatingGene->inputNeuron, neurons.size(), 1.0));
    connections.push_back(ConnectionGene::New(neurons.size(),
                                              mutatingGene->outputNeuron,
                                              mutatingGene->getWeight()));

    return make_pair(connections[connections.size() - 2], connections.back());
}

/* mutate a given percentage of connections */
void Genotype::mutateConnections(double percent) {
    
    const unsigned size = percent * connections.size();

    for (unsigned i = 0; i < size; i++)
        connections[rand128(rand) % connections.size()]->mutate();
}
