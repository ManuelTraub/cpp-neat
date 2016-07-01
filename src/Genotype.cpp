#include "Genotype.h"
#include <iterator>

using namespace NEAT;

/* global random value to mutate this */
rand64_t *Genotype::rand = new_rand64_t(time(NULL));

/* mutate this by inserting a new hidden neuron */
bool Genotype::mutateInsertNeuron() {

    auto random_it = std::next(std::begin(connections), rand64(rand) % connections.size());
    ConnectionGene &mutatingGene = random_it->second;

    /* stop if no neuron can be inserted */
    if (!mutatingGene.canBeSplitted())
        return false;

    /* stop if the gene is disabled */
    if (!mutatingGene.isEnabled())
        return false;

    /* stop if the connection is between two output neurons */
    if (mutatingGene.getInput().getType()  == NeuronType::OUTPUT &&
        mutatingGene.getOutput().getType() == NeuronType::OUTPUT)
        return false;

    /* to insert a NeuronGene without interfering to much with this
     * split an allready existing connecting into two parts
     * set the weight of the first part to 1 and the weight of the
     * second part to the weight of the splitted connection
     */
    NeuronGene neuron = history.insertNeuron(mutatingGene);

    /* stop if the neuron isn't a new neuron */
    if (neurons.count(neuron.getID()))
        return false;

    /* safe the new neuron otherwise */
    neurons.insert({neuron.getID(), neuron});

    /* disable the old connection */
    mutatingGene.disable();

    /* create the new genes within the gene history */
    ConnectionGene connection1 = history.insertConnection(mutatingGene.getInput(), neuron);
    ConnectionGene connection2 = history.insertConnection(neuron, mutatingGene.getOutput());

    /* modifiy the weights according to minimize the impact of the inserted neuron */
    connection1.setWeight(1.0);
    connection2.setWeight(mutatingGene.getWeight());

    /* store the new genes */
    if (connections.count(connection1.getID()) == 0)
        connections.insert({connection1.getID(), connection1});

    if (connections.count(connection2.getID()) == 0)
        connections.insert({connection2.getID(), connection2});

    /* set the generation number of this */
    if (generation < connections.at(connection1.getID()).getGeneration())
        generation = connections.at(connection1.getID()).getGeneration();

    if (generation < connections.at(connection2.getID()).getGeneration())
        generation = connections.at(connection2.getID()).getGeneration();

    if (generation < neuron.getGeneration())
        generation = neuron.getGeneration();
    
    return true;
}

/* mutate this by inserting a new connection */
bool Genotype::mutateInsertConnection() {
    
    auto random_it1 = std::next(std::begin(neurons), rand64(rand) % neurons.size());
    auto random_it2 = std::next(std::begin(neurons), rand64(rand) % neurons.size());
    const NeuronGene &inputNeuron  = random_it1->second;
    const NeuronGene &outputNeuron = random_it2->second;

    /* the output neuron cant be an input or bias */
    if (outputNeuron.getType() == NeuronType::INPUT ||
        outputNeuron.getType() == NeuronType::BIAS)
        return false;

    /* create the new conncetion */
    ConnectionGene connection = history.insertConnection(inputNeuron, outputNeuron);

    /* stop if the connection allready exists */
    if (connections.count(connection.getID()) != 0)
        return false;

    /* randomly mutate connection (incase of historical copy) */
    connection.mutate();

    /* store the new connection */
    connections.insert({connection.getID(), connection});

    if (generation < connection.getGeneration())
        generation = connection.getGeneration();

    return true;
}


/* the perturbe weight range */
double Genotype::perturbeWeightRange[2] = { -0.5, 0.5 };

/* the propability of reasining a gene a new random value during mutation */
double Genotype::hardMutationPropability = 0.1;

// TODO experiment with lovering the propability of mutating older genes
/* mutate a given percentage of connections */
void Genotype::mutateConnections(double percent) {
    
    const unsigned size = percent * connections.size();

    for (unsigned i = 0; i < size; i++) {
        auto random_it = std::next(std::begin(connections), rand64(rand) % connections.size());

        if (rand_range_d64(rand, 0, 1.0) < hardMutationPropability)
            random_it->second.mutate();
        else
            random_it->second.mutate(perturbeWeightRange);
    }

    if (size > 0)
        generation = GenerationCounter::get();
}

/* mutate this by disabeling a connection */
bool Genotype::mutateDisableConnection() {

    const unsigned index = rand64(rand) % connections.size();
    ConnectionGene &mutatingGene = connections[index];

    if (!mutatingGene.isEnabled())
        return false;

    mutatingGene.disable();
    
    if (generation < mutatingGene.getGeneration())
        generation = mutatingGene.getGeneration();

    return true;
}

/* create a new Genotype for the given number of input and output neurons */
Genotype::Genotype(const unsigned inputs, const unsigned outputs) : 
                   history(GeneHistory::getInstance()), generation(GenerationCounter::get()) {
    
    /* create the input neurons */
    for (unsigned i = 0; i < inputs; i++) {
        NeuronGene neuron = history.insertNeuron(NeuronType::INPUT, i);
        neurons.insert({neuron.getID(), neuron});
    }

    /* Add one Bias neuron to the inputs */
    NeuronGene neuron = history.insertNeuron(NeuronType::BIAS, 0);
    neurons.insert({neuron.getID(), neuron});

    /* create the output neurons */
    for (unsigned i = 0; i < outputs; i++) {
        NeuronGene neuron = history.insertNeuron(NeuronType::OUTPUT, i);
        neurons.insert({neuron.getID(), neuron});
    }
}

/* creat a new Genotype mixing the genes of this with other */
Genotype::Genotype(const Genotype &fitter, const Genotype &other) : 
                   history(GeneHistory::getInstance()), generation(0) {

    this->neurons = fitter.neurons;
    this->connections.reserve(fitter.connections.size());

    /* actualize the generation number */
    for (const auto &e : this->neurons)
        if (generation < this->neurons.at(e.first).getGeneration())
            generation = this->neurons.at(e.first).getGeneration();

    for (const auto &e : fitter.connections) {

        /* if a connection with the same historical id exists in other
         * choose a random weight from fitter or other
         */
        if (other.connections.count(e.first) != 0) {
            if (rand64(rand) % 2 == 1)
                this->connections[e.first] = other.connections.at(e.first);
            else
                this->connections[e.first] = fitter.connections.at(e.first);

        /* use only missing genes from the fitter individual */
        } else 
            this->connections[e.first] = fitter.connections.at(e.first);

        /* actualize the generation number */
        if (generation < this->connections.at(e.first).getGeneration())
            generation = this->connections.at(e.first).getGeneration();
    }
}

/* the gene multiplayer for calculating similarity */
double Genotype::geneMultiplier = 1.0;

/* avg weight diference multiplier when calculating the similarity */
double Genotype::weightMultiplier = 1.0;

/* calculates the similarity with this and other */
double Genotype::similarity(const Genotype &other) const {
    
    double geneSimilarity = this->neurons.size() + this->connections.size() +
                            other.neurons.size() + other.connections.size();

    double weightSimilarity = 0.0;

    for (const auto &e : neurons)
        if (other.neurons.count(e.first))
            geneSimilarity -= 2; 

    for (const auto &e : connections) {
        if (other.connections.count(e.first)) {
            geneSimilarity   -= 2;
            weightSimilarity += absDiff(e.second.getWeight(), 
                                        other.connections.at(e.first).getWeight());
        }
    }
                          
    // TODO experiment with nominalisation for genome length
    //      (geneSimilarity + weightSimilarity) / nGenes
    //      this should put the similarity in realation with Network size
    //      (in a giant network smal changes are cuses not a big dissimilarity as in a smal network)
    return geneMultiplier * geneSimilarity + weightMultiplier * weightSimilarity;
}

#ifdef __DEBUG__ 
NeuronGene &Genotype::addNeuron(const ConnectionGene &gene) {

    const NeuronGene neuron = history.insertNeuron(gene);

    if (neurons.count(neuron.getID()) == 0) 
        neurons.insert({neuron.getID(), neuron});

    if (generation < neurons[neuron.getID()].getGeneration())
        generation = neurons[neuron.getID()].getGeneration();

    return neurons[neuron.getID()];
}
ConnectionGene &Genotype::addConnection(const NeuronGene &gene1, const NeuronGene &gene2) {

    const ConnectionGene connection = history.insertConnection(gene1, gene2);

    if (connections.count(connection.getID()) == 0) 
        connections.insert({connection.getID(), connection});

    if (generation < connections[connection.getID()].getGeneration())
        generation = connections[connection.getID()].getGeneration();

    return connections[connection.getID()];
}
#endif
