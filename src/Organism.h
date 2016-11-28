/**
 * A Organism with a specific genome wich belongs to a
 * specific species and has a specific fittnes
 */
#ifndef __INDIVIDUUM_H__
#define __INDIVIDUUM_H__

#include "Genotype.h"
#include "Phenotype.h"
#include <memory>

using std::shared_ptr;

#define fitterGenome(o1, o2) ((o1.fitness > o2.fitness) ? o1.genome : o2.genome)
#define weakerGenome(o1, o2) ((o1.fitness > o2.fitness) ? o2.genome : o1.genome)
#define biggerGenome(o1, o2)  ((o1.genome.getConnections().size() > o2.genome.getConnections().size()) ? o1.genome : o2.genome)
#define smallerGenome(o1, o2) ((o1.genome.getConnections().size() > o2.genome.getConnections().size()) ? o2.genome : o1.genome)

namespace NEAT {

    class Organism {

        private:

            /* reference to the options of the whole population */
            Opts &opts;

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
            Organism(const unsigned inputs, const unsigned outputs, Opts &opts) :
                opts(opts.freeze()),
                genome(Genotype(inputs, outputs, opts)), 
                fitness(std::nan("Organism")), 
                alive(true) { }

            /* creating a new Organism by cloning this */
            Organism clone() {
                Organism newClone = *this;
                newClone.fitness = std::nan("Organism"); 
                newClone.alive = true;

                return newClone;
            }

            /* creat a new Organism mixing the genes of the two given Organisms */
            Organism(const Organism &parent1, const Organism &parent2, Opts &opts) : 
                opts(opts.freeze()),
                genome((parent1.fitness == parent2.fitness) ? 
                        Genotype(smallerGenome(parent1, parent2), 
                                 biggerGenome(parent1, parent2),
                                 opts) :
                        Genotype(fitterGenome(parent1, parent2), 
                                 weakerGenome(parent1, parent2),
                                 opts)),
                fitness(std::nan("Organism")), 
                alive(true) { 
                
                assert(!std::isnan(parent1.fitness) && !std::isnan(parent2.fitness));
            }

            /* calculates the fitness of this */
            void calculateFittness(vector<vector<double>> inputs, 
                                   vector<vector<double>> outputs);


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
            Genotype getGenome() const { return genome; }

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

#undef weakerGenome
#undef fitterGenome
#undef smallerGenome
#undef biggerGenome

#endif /* __INDIVIDUUM_H__ */
