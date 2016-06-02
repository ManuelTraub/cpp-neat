#include "Genotype.h"

using namespace NEAT;
using std::make_pair;

/* global random value to mutate this */
rand64_t *Genotype::rand = new_rand64_t(time(NULL));

/* the HexTree storing all created Connections */
ConnectionTree Genotype::globalConnections;

/* global lock */
pthread_mutex_t Genotype::mutex = PTHREAD_MUTEX_INITIALIZER;

/* mutate this by inserting a new hidden neuron */
bool Genotype::mutateInsertNeuron() {

    const unsigned index = rand64(rand) % connections.size();
    ConnectionGene *mutatingGene = connections[index];

    /* stop if no neuron can be inserted */
    if (!mutatingGene->canBeSplitted())
        return false;

    /* stop if the connection is a self recurent connection */
    if (mutatingGene->getInput() == mutatingGene->getOutput())
        return false;

    /* stop if the connection is between two output neurons */
    if (mutatingGene->getInput().getType() == NeuronType::OUTPUT &&
        mutatingGene->getOutput().getType() == NeuronType::OUTPUT)
        return false;

    /* to insert a NeuronGene without interfering to much with this
     * split an allready existing connecting into two parts
     * set the weight of the first part to 1 and the weight of the
     * second part to the weight of the splitted connection
     */
    NeuronGene *neuron = mutatingGene->split();

    /* try to insert the new NeuronGene if its not allready eveloped */
    NeuronGene *newNeuron = neuronsTree.insert(neuron);
    
    /* stop if the neuron isn't a new neuron */
    if (newNeuron != neuron) {
        delete neuron;
        return false;
    }

    /* safe the new neuron otherwise */
    neurons.push_back(newNeuron);

    /* disable the old connection */
    mutatingGene->disable();

    /* create the two new connections replacing the old connection */
    ConnectionGene *connection1 = new ConnectionGene(mutatingGene->getInput(), *newNeuron);
    ConnectionGene *connection2 = new ConnectionGene(*newNeuron, mutatingGene->getOutput());

    /* insert the new genes into the global gene pool 
     * copy historical ids if the genes allready in the pool
     */
    pthread_mutex_lock(&mutex);
    *connection1 = *globalConnections.insert(connection1);
    *connection2 = *globalConnections.insert(connection2);
    pthread_mutex_unlock(&mutex);

    /* modifiy the weights according to minimize the impact of the inserted neuron */
    connection1->setWeight(1.0);
    connection2->setWeight(mutatingGene->getWeight());

    /* store the new genes */
    ConnectionGene *newConnection1 = connectionsTree.insert(connection1);
    ConnectionGene *newConnection2 = connectionsTree.insert(connection2);
    connections.push_back(connection1);
    connections.push_back(connection2);

    assert(newConnection1 == connection1);
    assert(newConnection2 == connection2);

    return true;
}

/* mutate this by inserting a new connection */
bool Genotype::mutateInsertConnection() {
    
    const unsigned inputIndex  = rand64(rand) % neurons.size();
    const unsigned outputIndex = rand64(rand) % neurons.size();
    NeuronGene *inputNeuron  = neurons[inputIndex];
    NeuronGene *outputNeuron = neurons[outputIndex];

    /* the output neuron cant be an input or bias */
    if (outputNeuron->getType() == NeuronType::INPUT ||
        outputNeuron->getType() == NeuronType::BIAS)
        return false;

    /* create the new conncetion */
    ConnectionGene *connection = new ConnectionGene(*inputNeuron, *outputNeuron);

    /* check if the connection allready exists in this Genotype */
    ConnectionGene *newConnection = connectionsTree.insert(connection);

    /* stop if the connection allready exists */
    if (newConnection != connection) {
        delete connection;
        return false;
    }

    /* copy historical id of allready existing connection */
    pthread_mutex_lock(&mutex);
    *connection = *globalConnections.insert(connection);
    pthread_mutex_unlock(&mutex);

    /* randomly mutate connection (incase of copy) */
    connection->mutate();

    /* store the new connection */
    connections.push_back(connection);

    return true;
}

/* mutate a given percentage of connections */
void Genotype::mutateConnections(double percent) {
    
    const unsigned size = percent * connections.size();

    for (unsigned i = 0; i < size; i++)
        connections[rand64(rand) % connections.size()]->mutate();
}

/* mutate this by disabeling a connection */
bool Genotype::mutateDisableConnection() {

    const unsigned index = rand64(rand) % connections.size();
    ConnectionGene *mutatingGene = connections[index];

    if (!mutatingGene->isEnabled())
        return false;

    mutatingGene->disable();
    return true;
}

/* create a new Genotype for the given number of input and output neurons */
Genotype::Genotype(const unsigned inputs, const unsigned outputs) {
    
    const uint64_t inStepSize  = UINT64_MAX / inputs;
    const uint64_t outStepSize = UINT64_MAX / ((outputs > 1) ? (outputs - 1) : 1);

    /* create the input neurons */
    for (unsigned i = 0; i < inputs; i++) {
        NeuronGene *const neuron = new NeuronGene(NeuronType::INPUT, 0, i * inStepSize);
        neurons.push_back(neuron);
        neuronsTree.insert(neuron);
    }

    /* Add one Bias neuron to the inputs */
    NeuronGene *const neuron = new NeuronGene(NeuronType::BIAS, 0, UINT64_MAX);
    neurons.push_back(neuron);
    neuronsTree.insert(neuron);

    /* create the output neurons */
    for (unsigned i = 0; i < outputs; i++) {
        NeuronGene *const neuron = new NeuronGene(NeuronType::OUTPUT, UINT64_MAX, i * outStepSize);
        neurons.push_back(neuron);
        neuronsTree.insert(neuron);
    }

}

/* destructor */
Genotype::~Genotype() {
    for (auto &neuron : neurons)
        delete neuron;

    for (auto &connection : connections)
        delete connection;
}

#ifdef __DEBUG__ 
void Genotype::addNeuron(NeuronGene *gene) {
    NeuronGene *newGene = neuronsTree.insert(gene);
    if (newGene == gene)
        neurons.push_back(gene);
}
void Genotype::addConnection(ConnectionGene *gene) {
    ConnectionGene *newGene = connectionsTree.insert(gene);
    if (newGene == gene) {
        pthread_mutex_lock(&mutex);
        *gene = *globalConnections.insert(gene);
        pthread_mutex_unlock(&mutex);

        connections.push_back(gene);
    }
}
void Genotype::clearGlobalConnections() {
    globalConnections.clear();
}
#endif
