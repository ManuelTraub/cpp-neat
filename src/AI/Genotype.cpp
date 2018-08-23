#include "Genotype.h"
#include "Opts.h"
#include <iterator>
#include <assert.h>
#include <unistd.h>

using namespace NEAVC;
using namespace AI;

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

    if (neuron.getFunction() == ActivationFunction::SPIKING)
        connection1.setWeight(145);

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
        outputNeuron.getType() == NeuronType::BIAS  ||
        outputNeuron.getType() == NeuronType::RAND_BIAS)
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

/* mutate this by creating a new structure */
bool Genotype::mutateInsertStructure() {

    /* return if this doesnt uses structure genes */
    if (structures.empty())
        return false;
    
    auto random_it = std::next(std::begin(structures), rand64(rand) % structures.size());
    const StructureGene &baseStructure = random_it->second;

    /* regest a new adjacent StructureGene from the GeneHistory */
    StructureGene newStructure = this->geneHistory.insertStructure(baseStructure);

    /* stop if the structure allready exists */
    if (structures.count(newStructure.getID()) != 0)
        return false;

    /* randomly set the type of the new structure */
    if (rand_range_d64(rand, 0.0, 1.0) < opts.sensorTissuePropability())
        newStructure.setType(TissueType::SENSOR);
    else if (rand_range_d64(rand, 0.0, 1.0) < opts.muscleTissuePropability())
        newStructure.setType(TissueType::MUSCLE);

    /* add a new touch sensor for the new structure */
    NeuronGene ts = geneHistory.insertNeuron(NeuronType::INPUT, newStructure.getIndex());
    neurons.insert({ts.getID(), ts});
    geneLinks.push_back(SEGeneLink(ts, SensorEffector::TOUCH_SENSOR, newStructure.getIndex()));

    /* connect the touch sensor to all outputNeurons */
    this->connectInput(ts);
    
    /* add a new output Neuron for the new muscle if needed */
    if (newStructure.getType() == TissueType::MUSCLE) {

        /* add the muscle effector */
        const NeuronGene muscleEffector = this->geneHistory.insertNeuron(
            NeuronType::OUTPUT, 
            newStructure.getIndex()
        );
        neurons.insert({muscleEffector.getID(), muscleEffector});
        geneLinks.push_back(SEGeneLink(
            muscleEffector, 
            SensorEffector::MUSCLE_EFFECTOR, 
            newStructure.getIndex()
        ));

        /* connect all inputNeurons to the new muscle */
        this->connectOutput(muscleEffector);
    }

    /* store the new structure */
    this->structures.insert({newStructure.getID(), newStructure});

    return true;
}

/* removes hidden neurons without connections */
void Genotype::cleanupNeuorns() {

    /* delete all neurons without connections */
    vector<uint64_t> neuronIDs;
    for (auto &n: neurons) {
        if (n.second.getType() == NeuronType::HIDDEN) {

            bool noConnections = true;
            for (auto &c: connections) {
                if (c.second.getInput().getID()  == n.second.getID() ||
                    c.second.getOutput().getID() == n.second.getID()) {
                    
                    noConnections = false;
                    break;
                }
            }

            if (noConnections)
                neuronIDs.push_back(n.second.getID());
        }
    }
}

/* mutate this by deleting a new hidden neuron */
bool Genotype::mutateDeleteNeuron() {

    auto random_it = std::next(std::begin(neurons), rand64(rand) % neurons.size());
    const NeuronGene &neuron  = random_it->second;

    /* stop if we don't have a hidden neuron */
    if (neuron.getType() != NeuronType::HIDDEN)
        return false;

    /* delete all connections from and to the selected neuron */
    vector<uint64_t> connectionIDs;
    for (auto &e: connections) {
        if (e.second.getInput().getID()  == neuron.getID() ||
            e.second.getOutput().getID() == neuron.getID()) {

            connectionIDs.push_back(e.second.getID());
        }
    }
    for (auto &id: connectionIDs)
        connections.erase(id);

    /* remove neurons without connections */
    this->cleanupNeuorns();

    return true;
}

/* mutate this by creating a new connection */
bool Genotype::mutateDeleteConnection() {

    /* stop if there are no connections */
    if (connections.empty())
        return false;

    auto random_it = std::next(std::begin(connections), rand64(rand) % connections.size());
    ConnectionGene &connection = random_it->second;

    /* delete the connection */
    connections.erase(connection.getID());
    
    /* remove neurons without connections */
    this->cleanupNeuorns();

    return true;
}

/* mutate this by creating a new structure */
bool Genotype::mutateDeleteStructure() {
    
    // TODO remove structure und dependent outputs / inputs
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

        if (rand_range_d64(rand, 0.0, 1.0) < opts.mutateFunctionPropability())
            if (e.second.getType() != NeuronType::OUTPUT || 
                opts.outputActivationFunction() == ActivationFunction::LENGTH)
            e.second.mutateFunction();
    }

    generation = GenerationCounter::get();
}

/* mutate the structures of this */
void Genotype::mutateStructureType() {

    /* return if this doesnt uses structure genes */
    if (structures.empty())
        return;

    /* select random gene for mutation */
    auto random_it = std::next(std::begin(structures), rand64(rand) % structures.size());
    StructureGene &gene = random_it->second;

    /* mutate this by adding a muscle */
    if (gene.getType() == TissueType::TISSUE) {
        gene.setType(TissueType::MUSCLE);

        /* add a new output Neuron for the new muscle if needed */
        if (gene.getType() == TissueType::MUSCLE) {

            /* add the muscle effector */
            const NeuronGene muscleEffector = this->geneHistory.insertNeuron(
                NeuronType::OUTPUT, 
                gene.getIndex()
            );
            neurons.insert({muscleEffector.getID(), muscleEffector});
            geneLinks.push_back(SEGeneLink(
                muscleEffector, 
                SensorEffector::MUSCLE_EFFECTOR, 
                gene.getIndex()
            ));

            /* connect all inputNeurons to the new muscle */
            this->connectOutput(muscleEffector);
        }
    }

    generation = GenerationCounter::get();
}
void Genotype::mutateStructureSize() {

    for (auto &e : this->structures) {

        /* lower the propability of mutating older genes */
        if (rand_range_d64(rand, 0, 1) < 
            1.0 / (e.second.age() + 1) + opts.ageingPropabilityOffset()) {

            if (rand_range_d64(rand, 0, 1) < opts.hardMutationPropabilitySize()) {
                e.second.mutateSet({opts.mutationSetSizeRangeStart(), 
                                    opts.mutationSetSizeRangeEnd()});
            } else {
                e.second.mutatePertube(
                    {
                        opts.perturbeSizeRangeStart(), 
                        opts.perturbeSizeRangeEnd()
                    },
                    {
                        opts.mutationMaxPertubeSizeRangeStart(), 
                        opts.mutationMaxPertubeSizeRangeEnd()
                    }
                );
            }
        }
    }

    generation = GenerationCounter::get();
}

/* create a new Genotype with minimal structure */
Genotype::Genotype(Opts &opts, GeneHistory &geneHistory) : 
                   opts(opts.freeze()), 
                   geneHistory(geneHistory),
                   generation(GenerationCounter::get()) {

    assert(opts.allowRecurentConnections());
    
    /* add one output neuron */
    NeuronGene neuron = geneHistory.insertNeuron(NeuronType::OUTPUT, 0);
    neurons.insert({neuron.getID(), neuron});

    /* Add one Bias neuron to the inputs */
    NeuronGene bias = geneHistory.insertNeuron(NeuronType::BIAS, 0);
    neurons.insert({bias.getID(), bias});

    /* add three input neurons for the energy */
    NeuronGene energyInput1 = geneHistory.insertNeuron(NeuronType::INPUT, 0);
    NeuronGene energyInput2 = geneHistory.insertNeuron(NeuronType::INPUT, 1);
    NeuronGene energyInput3 = geneHistory.insertNeuron(NeuronType::INPUT, 2);
    neurons.insert({energyInput1.getID(), energyInput1});
    neurons.insert({energyInput2.getID(), energyInput2});
    neurons.insert({energyInput3.getID(), energyInput3});
    geneLinks.push_back(SEGeneLink(energyInput1, SensorEffector::ENERGY_SENSOR));
    geneLinks.push_back(SEGeneLink(energyInput2, SensorEffector::ENERGY_SENSOR));
    geneLinks.push_back(SEGeneLink(energyInput2, SensorEffector::ENERGY_SENSOR));

    /* the minimal structure consists of a single musce tissue atached to the brain */
    StructureGene brain  = geneHistory.insertStructure(TissueType::NEURON);
    StructureGene muscle = geneHistory.insertStructure(TissueType::MUSCLE);
    structures.insert({brain.getID(), brain});
    structures.insert({muscle.getID(), muscle});

    /* add one muscle output neuron */
    const TOGridIndex muscleIndex = muscle.getIndex();
    NeuronGene muscleNeuron = geneHistory.insertNeuron(NeuronType::OUTPUT, muscleIndex);
    neurons.insert({muscleNeuron.getID(), muscleNeuron});
    geneLinks.push_back(SEGeneLink(muscleNeuron, SensorEffector::MUSCLE_EFFECTOR, muscleIndex));

    /* add one touch sensor input for each structure */
    NeuronGene muscleTS = geneHistory.insertNeuron(NeuronType::INPUT, muscleIndex);
    neurons.insert({muscleTS.getID(), muscleTS});

    NeuronGene brainTS = geneHistory.insertNeuron(NeuronType::INPUT, TOGridIndex());
    neurons.insert({brainTS.getID(), brainTS});

    geneLinks.push_back(SEGeneLink(muscleTS, SensorEffector::TOUCH_SENSOR, muscleIndex));
    geneLinks.push_back(SEGeneLink(brainTS, SensorEffector::TOUCH_SENSOR, brain.getIndex()));

    if (opts.netActivationFunction() == ActivationFunction::SPIKING) {

        /* set the bias for or output neurons in the range 0 to 20 */
        ConnectionGene biasConnection = geneHistory.insertConnection(bias, muscleNeuron);
        biasConnection.mutateSet({0.0, 20.0});
        connections.insert({biasConnection.getID(), biasConnection});

    } else {

        /* fully connect all inputs with all outputs */
        this->createInitialConnections(true);
    }
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

    /* create a random bias for each output if wanted output neurons */
    vector<NeuronGene> randomBiases;
    if (opts.useRandomBiases()) {
        for (unsigned i = 0; i < numOutputs; i++) {
            NeuronGene neuron = geneHistory.insertNeuron(NeuronType::RAND_BIAS, i);
            neurons.insert({neuron.getID(), neuron});
            randomBiases.push_back(neuron);
        }
    }

    /* create the output neurons */
    vector<NeuronGene> outputs;
    for (unsigned i = 0; i < numOutputs; i++) {
        NeuronGene neuron = geneHistory.insertNeuron(NeuronType::OUTPUT, i);
        neurons.insert({neuron.getID(), neuron});
        outputs.push_back(neuron);
    }

    /* connect each random bias to one output */
    if (opts.useRandomBiases()) {
        for (unsigned i = 0; i < numOutputs; i++) {
            
            const ConnectionGene connection = geneHistory.insertConnection(randomBiases[i], outputs[i]);

            if (connections.count(connection.getID()) == 0) 
                connections.insert({connection.getID(), connection});

            if (generation < connections.at(connection.getID()).getGeneration())
                generation = connections.at(connection.getID()).getGeneration();

            connections.at(connection.getID()).setWeight(1);
        }
    }

}

/* recombinate using the weight of similar genes from a random parrent */
void Genotype::recombinateRandomChoose(const Genotype &fitter, const Genotype &other) {

    this->neurons.reserve(fitter.neurons.size());
    this->connections.reserve(fitter.connections.size());
    this->structures.reserve(fitter.structures.size());
    this->geneLinks = fitter.geneLinks;

    /* recombinate structure */
    for (const auto &e : fitter.structures) {

        /* if a structure with the same historical id exists in other
         * choose a random size from fitter or other
         */
        if (other.structures.count(e.first) != 0) {
            if (rand64(rand) % 2 == 1)
                this->structures.insert({e.first, other.structures.at(e.first)});
            else
                this->structures.insert({e.first, fitter.structures.at(e.first)});

             this->structures.at(e.first).setType(e.second.getType());

        /* use only missing genes from the fitter individual */
        } else 
            this->structures.insert({e.first, e.second});

        /* actualize the generation number */
        if (generation < this->structures.at(e.first).getGeneration())
            generation = this->structures.at(e.first).getGeneration();
    }

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
    this->structures  = fitter.structures;
    this->geneLinks   = fitter.geneLinks;

    /* recombinate structures */
    for (const auto &e : fitter.structures) {

        /* if a structure with the same historical id exists in other
         * average the size of both
         */
        if (other.structures.count(e.first) != 0) {
            this->structures.at(e.first).setSize((
                fitter.structures.at(e.first).getSize() + 
                other.structures.at(e.first).getSize()
            ) / 2.0);
        }

        /* actualize the generation number */
        if (generation < this->structures.at(e.first).getGeneration())
            generation = this->structures.at(e.first).getGeneration();
    }

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

            if (rand64(rand) % 2 == 1)
                this->neurons.at(e.first).setFunction(other.neurons.at(e.first).getFunction());
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

    /* size similarity */
    uint64_t nSizes = 0;
    double sizeSimilarity = 0.0;
    for (const auto &e : this->structures) {
        if (other.structures.count(e.first)) {

            sizeSimilarity += absDiff(
                e.second.getSize(),
                other.structures.at(e.first).getSize()
            );
            nSizes++;
        }
    }
    
    /* gradient similarity */
    uint64_t nNeurons = 0;
    double gradientSimilarity = 0.0;
    double functionSimilarity = 0.0;
    for (const auto &e : this->neurons) {
        if (e.second.getType() != NeuronType::INPUT     && 
            e.second.getType() != NeuronType::BIAS      &&
            e.second.getType() != NeuronType::RAND_BIAS &&
            other.neurons.count(e.first)) {

            gradientSimilarity += absDiff(
                e.second.getGradient(),
                other.neurons.at(e.first).getGradient()
            );
            
            if (e.second.getFunction() != other.neurons.at(e.first).getFunction())
                functionSimilarity += 1;

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

    /* structure similarity */
    uint64_t structureSimilarity = this->structures.size() + other.structures.size();
    for (const auto & e: this->structures) {
        if (other.structures.count(e.first) && 
            e.second.getType() == other.structures.at(e.first).getType()) {

            structureSimilarity -= 2;
        }
    }


    double similarity = 0;    
    similarity += opts.geneMultiplier() * geneSimilarity;

    if (nWeights > 0)
        similarity += opts.weightMultiplier() * (weightSimilarity / nWeights);

    if (nNeurons > 0) {
        similarity += opts.gradientMultiplier() * log(1 + gradientSimilarity / nNeurons);
        similarity += opts.functionMultiplier() * functionSimilarity;
    }

    if (nSizes > 0)
        similarity += opts.sizeMultiplier() * (sizeSimilarity / nSizes);

    similarity += opts.structureMultiplier() * structureSimilarity;

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

/* connects the bias to the the given (new) output */
void Genotype::connectToBias(NeuronGene o) {

    for (const auto &e : neurons) {
        if (e.second.getType() == NeuronType::BIAS) {

            const NeuronGene bias = e.second;

            ConnectionGene biasConnection = geneHistory.insertConnection(bias, o);
            biasConnection.mutateSet({0.0, 20.0});
            connections.insert({biasConnection.getID(), biasConnection});
        }
    }
}

/* connects all inputs to the the given (new) output */
void Genotype::connectOutput(NeuronGene o, bool connectBias) {

    if (opts.useRandomBiases()) {
        printf("implement rand bias for new outputs!!!\nyou need to create a random bias and connect it to the new output with a weight of 1.0 (or other for spicing)\n");
        exit(EXIT_FAILURE);
    }


    /* sort the Neurons by input and output connections */
    vector<NeuronGene> inputs;

    for (const auto &e : neurons) {
        if (e.second.getType() == NeuronType::INPUT ||
            (e.second.getType() == NeuronType::BIAS && connectBias))
            inputs.push_back(e.second);
    }

    /* optimal weight range for a neuron with n itputs is [-1/sqrt(n), 1/sqrt(n)] */
    const double wStart = -1.0 / sqrt(inputs.size());
    const double wEnd   =  1.0 / sqrt(inputs.size());

    for (const auto &i : inputs) {

        const ConnectionGene connection = geneHistory.insertConnection(i, o);

        if (connections.count(connection.getID()) == 0) 
            connections.insert({connection.getID(), connection});

        if (generation < connections.at(connection.getID()).getGeneration())
            generation = connections.at(connection.getID()).getGeneration();

        connections.at(connection.getID()).mutateSet({wStart, wEnd});
    }
}

/* conects the given (new) input to all outputs */
void Genotype::connectInput(NeuronGene i, bool connectBias) {

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

    for (const auto &o : outputs) {

        const ConnectionGene connection = geneHistory.insertConnection(i, o);
  
        if (connections.count(connection.getID()) == 0) 
            connections.insert({connection.getID(), connection});
  
        if (generation < connections.at(connection.getID()).getGeneration())
            generation = connections.at(connection.getID()).getGeneration();

        connections.at(connection.getID()).mutateSet({wStart, wEnd});
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

    const uint64_t structuresSize = structures.size();
    if (write(fd, &structuresSize, sizeof(uint64_t)) != sizeof(uint64_t))
        return false;

    for (const auto &e : structures)
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

    uint64_t structuresSize = 0;
    if (read(fd, &structuresSize, sizeof(uint64_t)) != sizeof(uint64_t))
        throw "IO Exception";

    for (unsigned i = 0; i < structuresSize; i++) {
        const StructureGene structure(fd);
        structures.insert({structure.getID(), structure});
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

#ifdef __DEBUG__ 
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
#endif
