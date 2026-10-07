/**
 * A Organism with a specific genome wich belongs to a
 * specific species and has a specific fittnes
 */
#ifndef __INDIVIDUUM_H__
#define __INDIVIDUUM_H__

#include "Genotype.h"
#include "GeneHistory.h"
#include "Phenotype.h"
#include <memory>

using std::shared_ptr;

namespace NEAT {

        /* abstract interface to calculate the fittness of a given Genome */
        struct FitnessCalculator {

            /* virtual fitness operator */
            virtual double operator () (const Genotype &genome) = 0;

            /* abstract constructor */
            virtual ~FitnessCalculator() { }

        };

        class Organism {

            private:

                /* reference to the options of the whole population */
                Opts &opts;

                /* reference to the GeneHistory of the whole population */
                GeneHistory &geneHistory;

                /* global random value */
                static rand64_t *rand;

                /* the genome of this Organism */
                Genotype genome;

                /* the original fitness of this Organism */
                double fitness;

                /* indicates that this Organism is alive */
                bool alive;

            public:

                /* create a new Organism for the given number of input and output neurons */
                Organism(
                    const unsigned inputs, 
                    const unsigned outputs, 
                    Opts &opts, 
                    GeneHistory &geneHistory
                );

                /* create a new Organism from the given genome */
                Organism(
                    Genotype &genome,
                    Opts &opts, 
                    GeneHistory &geneHistory
                );

                /* creating a new Organism by cloning this */
                Organism clone();

                /* creat a new Organism mixing the genes of the two given Organisms */
                Organism(
                    const Organism &parent1, 
                    const Organism &parent2, 
                    Opts &opts,
                    GeneHistory &geneHistory
                );

                /* calculates the fitness of this */
                void calculateFittness(
                    vector<vector<double>> inputs, 
                    vector<vector<double>> outputs
                );

                /* calculates the fitness of this using the given callback */
                void calculateFittness(FitnessCalculator *fitnessCalculator) {
                    if (std::isnan(fitness) || opts.alwaysRecalculateFitness())
                        this->fitness = (*fitnessCalculator)(this->genome);
                }

                /* returns the original fitness of this */
                double getFitness() const { 
                    assert(!std::isnan(fitness));
                    return fitness; 
                }

                /* mutates this */
                void mutate();

                /* returns whether this Organism is alive */
                bool isAlive() const { return alive; }

                /* kills this Organism */
                void kill() { 
                    assert(alive);
                    alive = false;
                }

                /* returns the similarity with this and the given genome */
                double similarity(const Organism &o) const {
                    return this->genome.similarity(o.genome);
                }

                /* returns a copy of the Genome of this */
                const Genotype &getGenome() const { return genome; }

                /* create all initial connections */
                void createInitialConnections(bool connectBias = true) {
                    this->genome.createInitialConnections(connectBias);
                }

#ifdef __DEBUG__
                void setFitness(double fitness) { this->fitness = fitness; }
#endif

        };

        /* compares two Organism based on theyr adjustedFitness fittness */
        bool orderOrganisms(const shared_ptr<Organism> &a, const shared_ptr<Organism> &b);

}

#endif /* __INDIVIDUUM_H__ */
