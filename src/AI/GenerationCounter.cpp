#include "GenerationCounter.h"

using namespace NEAVC;
using namespace AI;

/* the only instance of the GenerationCounter */
GenerationCounter GenerationCounter::onlyInstance;

/* get the number of the current generation */
uint64_t GenerationCounter::get() {
    return onlyInstance.generation;
}

#ifdef __DEBUG__
            
/* increases the generation counter */
void GenerationCounter::inc() {
    onlyInstance.generation++;
}
#endif
