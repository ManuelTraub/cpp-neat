#ifndef __PARALLEL_WORKER_H__
#define __PARALLEL_WORKER_H__

#include "../utils/Threads.h"
#include "Organism.h"

#include <deque>

using std::deque;

namespace NEAVC {

    namespace AI {
    
        class ParallelWorker: public Thread {
            
            private:

                /* the organisms to process */
                deque<shared_ptr<Organism>> &organisms;

                /* syncronization mutex */
                Mutex &mutex;

                /* the FitnessCalculator of this */
                FitnessCalculator *fitnessCalculator;

            public:

                /* thread function to override by child class */
                virtual void threadFunction(void *args);

                /* constructor */
                ParallelWorker(
                    deque<shared_ptr<Organism>> &organisms,
                    Mutex &mutex,
                    FitnessCalculator *fitnessCalculator
                ) : organisms(organisms), 
                    mutex(mutex), 
                    fitnessCalculator(fitnessCalculator) { }

        };

        class ParallelProcessor {

            private:

                /* the organisms to process */
                deque<shared_ptr<Organism>> organisms;

                /* syncronization mutex */
                Mutex mutex;

                /* the FitnessCalculator of this */
                FitnessCalculator *fitnessCalculator;

                /* the worker threads of this */
                vector<ParallelWorker> worker;

            public:

                /* adds the given vector of Organisms */
                void add(vector<shared_ptr<Organism>> organisms);

                /* wait for all workers */
                void join();

                /* start the worker of this */
                void start();

                /* constructor */
                ParallelProcessor(const unsigned numThreads, FitnessCalculator *fitnessCalculator);

        };
    }
};

#endif /* __PARALLEL_WORKER_H__ */
