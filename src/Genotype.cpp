#include "Genotype.h"
#include "Opts.h"
#include <iterator>
#include <assert.h>
#include <unistd.h>

using namespace NEAT;

/* global random value to mutate this */
rand64_t *Genotype::rand = new_rand64_t();

/* mutate this by inserting a new hidden neuron */
bool Genotype::mutateInsertNeuron() {

    /* stop if there are no connections */
    if (connections.empty())
        return false;

    auto random_it = std::next(std::begin(connections), rand64(rand) % connections.size());
    ConnectionGene &mutatingGene = random_it->second;

    /* stop if no neuron can be inserted */
    if (!mutatingGene.canBeSplitted())
        return false;

    /* stop if the gene is disabled */
    if (!mutatingGene.isEnabled())
        return false;

    /* stop if the connection is a bias connection */
    if (mutatingGene.getInput().getType() == NeuronType::BIAS &&
        !opts.allowNeuronIntoBiasConnectionInsert())
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
    NeuronGene neuron = geneHistory.insertNeuron(mutatingGene);

    /* stop if the neuron isn't a new neuron */
    if (neurons.count(neuron.getID()))
        return false;

    /* safe the new neuron otherwise */
    neurons.insert({neuron.getID(), neuron});

    /* disable the old connection */
    mutatingGene.disable();

    /* create the new genes within the gene history */
    ConnectionGene connection1 = geneHistory.insertConnection(mutatingGene.getInput(), neuron);
    ConnectionGene connection2 = geneHistory.insertConnection(neuron, mutatingGene.getOutput());

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

    /* stop if its a recurent connection and we arent allowet to create one */
    if (!opts.allowRecurentConnections() && 
        outputNeuron.getLayerID() <= inputNeuron.getLayerID())
        return false;

    /* create the new conncetion */
    ConnectionGene connection = geneHistory.insertConnection(inputNeuron, outputNeuron);

    /* stop if the connection allready exists */
    if (connections.count(connection.getID()) != 0)
        return false;

    /* randomly mutate connection (incase of historical copy) */
    connection.mutateSet({opts.creationWeightRangeStart(), opts.creationWeightRangeEnd()}); 

    /* store the new connection */
    connections.insert({connection.getID(), connection});

    if (generation < connection.getGeneration())
        generation = connection.getGeneration();

    return true;
}

/* mutate a given percentage of connections */
void Genotype::mutateConnections() {
    
    for (auto &e : this->connections) {

        /* lower the propability of mutating older genes */
        if (rand_range_d64(rand, 0, 1) < 
            1.0 / (e.second.age() + 1) + opts.ageingPropabilityOffset()) {

            if (rand_range_d64(rand, 0, 1) < opts.hardMutationPropabilityWeights()) {
                e.second.mutateSet({opts.mutationSetWeightRangeStart(), 
                                    opts.mutationSetWeightRangeEnd()});
            } else {
                e.second.mutatePertube(
                    {
                        opts.perturbeWeightRangeStart(), 
                        opts.perturbeWeightRangeEnd()
                    },
                    {
                        opts.mutationMaxPertubeWeightRangeStart(), 
                        opts.mutationMaxPertubeWeightRangeEnd()
                    }
                );
            }
        }
    }

    generation = GenerationCounter::get();
}

/* mutate a given percentage of the neurons of this */
void Genotype::mutateNeurons() {
    
    for (auto &e : this->neurons) {
        e.second.mutateGradient();
    }

    generation = GenerationCounter::get();
}

/* create a new Genotype for the given number of input and output neurons */
Genotype::Genotype(
    const unsigned numInputs, 
    const unsigned numOutputs, 
    Opts &opts,
    GeneHistory &geneHistory
) : 
    opts(opts.freeze()),
    geneHistory(geneHistory),
    generation(GenerationCounter::get()) {

    /* create the input neurons */
    for (unsigned i = 0; i < numInputs; i++) {
        NeuronGene neuron = geneHistory.insertNeuron(NeuronType::INPUT, i);
        neurons.insert({neuron.getID(), neuron});
    }

    /* Add one Bias neuron to the inputs */
    NeuronGene neuron = geneHistory.insertNeuron(NeuronType::BIAS, 0);
    neurons.insert({neuron.getID(), neuron});

    /* create the output neurons */
    for (unsigned i = 0; i < numOutputs; i++) {
        NeuronGene neuron = geneHistory.insertNeuron(NeuronType::OUTPUT, i);
        neurons.insert({neuron.getID(), neuron});
    }

}

/* recombinate using the weight of similar genes from a random parrent */
void Genotype::recombinateRandomChoose(const Genotype &fitter, const Genotype &other) {

    this->neurons.reserve(fitter.neurons.size());
    this->connections.reserve(fitter.connections.size());

    /* recombinate neurons */
    for (const auto &e : fitter.neurons) {

        /* if a neuron with the same historical id exists in other
         * choose a random gradient from fitter or other
         */
        if (other.neurons.count(e.first) != 0) {
            if (rand64(rand) % 2 == 1)
                this->neurons.insert({e.first, other.neurons.at(e.first)});
            else
                this->neurons.insert({e.first, fitter.neurons.at(e.first)});

        /* use only missing genes from the fitter individual */
        } else 
            this->neurons.insert({e.first, fitter.neurons.at(e.first)});

        /* actualize the generation number */
        if (generation < this->neurons.at(e.first).getGeneration())
            generation = this->neurons.at(e.first).getGeneration();
    }

    if (!opts.recombinateNeurons())
        this->neurons = fitter.neurons;

    /* recombinate connections */
    for (const auto &e : fitter.connections) {

        /* if a connection with the same historical id exists in other
         * choose a random weight from fitter or other
         */
        if (other.connections.count(e.first) != 0) {
            if (rand64(rand) % 2 == 1)
                this->connections.insert({e.first, other.connections.at(e.first)});
            else
                this->connections.insert({e.first, fitter.connections.at(e.first)});
            
            /* during mating a gene can be renabled */ 
            if (other.connections.at(e.first).isEnabled() == false ||
                this->connections.at(e.first).isEnabled() == false) {
                if (rand_range_d64(rand, 0, 1) < opts.geneReenablePropability()) 
                    this->connections.at(e.first).enable();
                else
                    this->connections.at(e.first).disable();
            }

        /* use only missing genes from the fitter individual */
        } else 
            this->connections.insert({e.first, fitter.connections.at(e.first)});

        /* actualize the generation number */
        if (generation < this->connections.at(e.first).getGeneration())
            generation = this->connections.at(e.first).getGeneration();
    }

}

/* recombinate averaging the weights of similar genes */
void Genotype::recombinateAverage(const Genotype &fitter, const Genotype &other) {

    this->neurons     = fitter.neurons;
    this->connections = fitter.connections;

    /* recombinate neurons */
    for (const auto &e : fitter.neurons) {

        /* if a neuron with the same historical id exists in other
         * average the gradient of both
         */
        if (other.neurons.count(e.first) != 0) {
            this->neurons.at(e.first).setGradient((
                fitter.neurons.at(e.first).getGradient() + 
                other.neurons.at(e.first).getGradient()
            ) / 2.0);
        }

        /* actualize the generation number */
        if (generation < this->neurons.at(e.first).getGeneration())
            generation = this->neurons.at(e.first).getGeneration();
    }

    if (!opts.recombinateNeurons())
        this->neurons = fitter.neurons;

    /* recombinate connections */
    for (const auto &e : fitter.connections) {

        /* if a connection with the same historical id exists in other
         * average the weight of both
         */
        if (other.connections.count(e.first) != 0) {
            this->connections.at(e.first).setWeight((
                fitter.connections.at(e.first).getWeight() +
                other.connections.at(e.first).getWeight()
            ) / 2.0);
            
            /* during mating a gene can be renabled */ 
            if (other.connections.at(e.first).isEnabled() == false ||
                this->connections.at(e.first).isEnabled() == false) {
                if (rand_range_d64(rand, 0, 1) < opts.geneReenablePropability()) 
                    this->connections.at(e.first).enable();
                else
                    this->connections.at(e.first).disable();
            }
        } 

        /* actualize the generation number */
        if (generation < this->connections.at(e.first).getGeneration())
            generation = this->connections.at(e.first).getGeneration();
    }
}

/* creat a new Genotype mixing the genes of this with other */
Genotype::Genotype(
    const Genotype &fitter, 
    const Genotype &other, 
    Opts &opts,
    GeneHistory &geneHistory
) : 
   opts(opts.freeze()), 
   geneHistory(geneHistory),
   generation(0) {

    if (rand_range_d64(rand, 0, 1) < opts.avgMatingPropability())
        this->recombinateAverage(fitter, other);
    else
        this->recombinateRandomChoose(fitter, other);
} 

/* calculates the similarity with this and other */
double Genotype::similarity(const Genotype &other) const {

    /* gradient similarity */
    uint64_t nNeurons = 0;
    double gradientSimilarity = 0.0;
    for (const auto &e : this->neurons) {
        if (e.second.getType() != NeuronType::INPUT     && 
            e.second.getType() != NeuronType::BIAS      &&
            other.neurons.count(e.first)) {

            gradientSimilarity += absDiff(
                e.second.getGradient(),
                other.neurons.at(e.first).getGradient()
            );

            nNeurons++;
        }
    }

    /* weith similarity */
    uint64_t nWeights       = 0;
    double geneSimilarity   = this->connections.size() + other.connections.size();
    double weightSimilarity = 0.0;

    for (const auto &e : this->connections) {
        if (other.connections.count(e.first)) {
            geneSimilarity   -= 2;
            weightSimilarity += absDiff(
                e.second.getWeight(), 
                other.connections.at(e.first).getWeight()
            );
            nWeights++;
        }
    }

    double similarity = 0;    
    similarity += opts.geneMultiplier() * geneSimilarity;

    if (nWeights > 0)
        similarity += opts.weightMultiplier() * (weightSimilarity / nWeights);

    if (nNeurons > 0) {
        similarity += opts.gradientMultiplier() * log(1 + gradientSimilarity / nNeurons);
    }

    assert(!std::isnan(similarity));
    return similarity;
}

/* create the initial connections which 
 * fully connects the input and output Neurons of this 
 */
void Genotype::createInitialConnections(bool connectBias) {

    /* sort the Neurons by input and output connections */
    vector<NeuronGene> inputs, outputs;

    for (const auto &e : neurons) {
        if (e.second.getType() == NeuronType::INPUT)
            inputs.push_back(e.second);
        else if (e.second.getType() == NeuronType::BIAS && connectBias)
            inputs.push_back(e.second);
        else if (e.second.getType() == NeuronType::OUTPUT)
            outputs.push_back(e.second);
    }

    /* optimal weight range for a neuron with n itputs is [-1/sqrt(n), 1/sqrt(n)] */
    const double wStart = -1.0 / sqrt(inputs.size());
    const double wEnd   =  1.0 / sqrt(inputs.size());

    for (const auto &i : inputs) {
        for (const auto &o : outputs) {

            const ConnectionGene connection = geneHistory.insertConnection(i, o);
  
            if (connections.count(connection.getID()) == 0) 
                connections.insert({connection.getID(), connection});
  
            if (generation < connections.at(connection.getID()).getGeneration())
                generation = connections.at(connection.getID()).getGeneration();

            connections.at(connection.getID()).mutateSet({wStart, wEnd});
        }
    }
}

/* saves this to the given file descriptor */
bool Genotype::save(const int fd) const {

    if (write(fd, &generation, sizeof(uint64_t)) != sizeof(uint64_t))
        return false;

    const uint64_t neuronsSize = neurons.size();
    if (write(fd, &neuronsSize, sizeof(uint64_t)) != sizeof(uint64_t))
        return false;

    for (const auto &e : neurons)
        if (!e.second.save(fd))
            return false;

    const uint64_t connectionsSize = connections.size();
    if (write(fd, &connectionsSize, sizeof(uint64_t)) != sizeof(uint64_t))
        return false;

    for (const auto &e : connections)
        if (!e.second.save(fd))
            return false;

    return true;
}

/* loads the given Genotype from the given file */
Genotype::Genotype(const int fd, Opts &opts, GeneHistory &geneHistory) :
    opts(opts.freeze()),
    geneHistory(geneHistory)
{

    if (read(fd, &generation, sizeof(uint64_t)) != sizeof(uint64_t))
        throw "IO Exception";

    uint64_t neuronsSize = 0;
    if (read(fd, &neuronsSize, sizeof(uint64_t)) != sizeof(uint64_t))
        throw "IO Exception";

    for (unsigned i = 0; i < neuronsSize; i++) {
        const NeuronGene neuron(fd);
        neurons.insert({neuron.getID(), neuron});
    }

    uint64_t connectionsSize = 0;
    if (read(fd, &connectionsSize, sizeof(uint64_t)) != sizeof(uint64_t))
        throw "IO Exception";

    for (unsigned i = 0; i < connectionsSize; i++) {
        const ConnectionGene connection(fd);
        connections.insert({connection.getID(), connection});
    }

}

/* number of enabled connection genes */
unsigned Genotype::numEnabledConnections() const {

    unsigned enabledConnections = 0;
    for (const auto &c : this->connections)
        if (c.second.isEnabled())
            enabledConnections++;

    return enabledConnections;
}

NeuronGene &Genotype::addNeuron(const ConnectionGene &gene) {

    const NeuronGene neuron = geneHistory.insertNeuron(gene);

    if (neurons.count(neuron.getID()) == 0) 
        neurons.insert({neuron.getID(), neuron});

    if (generation < neurons.at(neuron.getID()).getGeneration())
        generation = neurons.at(neuron.getID()).getGeneration();

    return neurons.at(neuron.getID());
}
ConnectionGene &Genotype::addConnection(const NeuronGene &gene1, const NeuronGene &gene2) {

    const ConnectionGene connection = geneHistory.insertConnection(gene1, gene2);

    if (connections.count(connection.getID()) == 0) 
        connections.insert({connection.getID(), connection});

    if (generation < connections.at(connection.getID()).getGeneration())
        generation = connections.at(connection.getID()).getGeneration();

    return connections.at(connection.getID());
}
