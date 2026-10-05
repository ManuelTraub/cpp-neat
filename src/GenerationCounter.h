/**
 * a global (singleton) Generation counter
 */
#ifndef __GENERATION_COUNTER_H__
#define __GENERATION_COUNTER_H__

#include <inttypes.h>

namespace NEAT {

        /* forward declaration */
        class Population;
        
        class GenerationCounter {

            friend class Population;

            private:

                /* the only instance of the GenerationCounter */
                static GenerationCounter onlyInstance;

                /* delete copy constructor to disalow copies of this */
                GenerationCounter(const GenerationCounter &) = delete;

                /* private constructor and destructor to make this a singleton */
                GenerationCounter() : generation(0) { }

                /* private destructor to avoid deletion of the singleton */
                ~GenerationCounter() { }

                /* the number of current generation */
                uint64_t generation;

            public:

                /* get the number of the current generation */
                static uint64_t get();

#ifdef __DEBUG__
                
                /* increases the generation counter */
                static void inc();
#endif

        };
}

#endif /* __GENERATION_COUNTER_H__ */
