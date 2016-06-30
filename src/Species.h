/**
 * Class enscapsulating Genomes representating different individuums within the same species
 */
#ifndef __SPECIES_H__
#define __SPECIES_H__

namespace NEAT {
    
    class Species {
        
        private:

            /* The oldest individuum of this species 
             * which is used as base for similarity calculation */
            Genomes *oldest;

            /* the Genomes of this */
            vector<Genotype *> genoms;

        public:
            
            /* creats a new species */
            Species() : oldest(NULL) { }

            /* get the number of Geneomes of this */
            unsigned size() const { return genoms.size(); }

            /* add a Genome at the end of this */
            void add(const Genotype &genome) { genoms.insert(&genome); }

            /* removes all individuals form this expept the oldest */
            void clear() { genomes.clear(); }

            /* recalculates the oldes genome */
            void refresh() {
                for (const auto &g : genomes)
                    if (oldes == NULL || oldest->genomeAge() < g->genomeAge())
                        oldest = g;
            }

            /* calculate the simmilarity of the given Genotype
             * to the oldest genome of this species 
             */
            double similarity(const Genotype &genome) const { 
                assert(oldest != NULL);
                return oldest->similarity(genome); 
            }

    };
}

#endif /* __SPECIES_H__ */
