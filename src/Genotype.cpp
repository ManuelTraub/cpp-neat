#include "Genotype.h"

using namespace NEAT;
using std::make_pair;

/* global random value to mutate this */
rand64_t *Genotype::rand = new_rand64_t(time(NULL));

/* global lock */
pthread_mutex_t Genotype::mutex = PTHREAD_MUTEX_INITIALIZER;

/* mutate this by inserting a new hidden neuron */
bool Genotype::mutateInsertNeuron() {

    const unsigned index = rand64(rand) % connections.size();
    ConnectionGene *mutatingGene = connections[index];

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
    *connection1 = *globalConnections.insert(connection2);
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
}

/* mutate a given percentage of connections */
void Genotype::mutateConnections(double percent) {
    
    const unsigned size = percent * connections.size();

    for (unsigned i = 0; i < size; i++)
        connections[rand64(rand) % connections.size()]->mutate();
}
