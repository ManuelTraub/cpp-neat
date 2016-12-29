#include "ParallelWorker.h"

using namespace NEAVC;
using namespace AI;

/* thread function to override by child class */
void ParallelWorker::threadFunction(void *args) {
    
    (void) args;

    while (!this->organisms.empty()) {
        
        shared_ptr<Organism> organism;
        mutex.lock();

        if (!this->organisms.empty()) {
            organism = this->organisms.front();
            this->organisms.pop_front();
        }

        mutex.unlock();

        if (organism != nullptr)
            organism->calculateFittness(fitnessCalculator);
    }
}

/* adds the given vector of Organisms */
void ParallelProcessor::add(vector<shared_ptr<Organism>> organisms) {
    mutex.lock();
    this->organisms.insert(this->organisms.end(), organisms.begin(), organisms.end());
    mutex.unlock();
}

/* wait for all workers */
void ParallelProcessor::join() {
    for (auto &w : worker)
        w.join();
}

void ParallelProcessor::start() {
    for (auto &w : worker)
        w.run();
}

/* constructor */
ParallelProcessor::ParallelProcessor(
    const unsigned numThreads, 
    FitnessCalculator *fitnessCalculator
) {
    this->fitnessCalculator = fitnessCalculator;

    for (unsigned i = 0; i < numThreads; i++) {
        this->worker.push_back(ParallelWorker(
            organisms,
            mutex,
            fitnessCalculator
        ));
    }
}
