#ifndef __PARALLEL_WORKER_H__
#define __PARALLEL_WORKER_H__

#include "utils/Threads.h"
#include "Organism.h"

#include <deque>
#include <atomic>
#include <chrono>
#include <thread>
#include <sstream>

using std::deque;

namespace NEAT {

    
        class ParallelWorker: public Thread {
            
            private:

                /* the organisms to process */
                deque<shared_ptr<Organism>> &organisms;

                /* syncronization mutex */
                Mutex &mutex;

                /* the FitnessCalculator of this */
                FitnessCalculator *fitnessCalculator;

                /* progress tracking */
                std::atomic<unsigned> &processedCount;

            public:

                /* thread function to override by child class */
                virtual void threadFunction(void *);

                /* constructor */
                ParallelWorker(
                    deque<shared_ptr<Organism>> &organisms,
                    Mutex &mutex,
                    FitnessCalculator *fitnessCalculator,
                    std::atomic<unsigned> &processedCount
                ) : organisms(organisms), 
                    mutex(mutex), 
                    fitnessCalculator(fitnessCalculator),
                    processedCount(processedCount) { }

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

                /* progress tracking */
                unsigned totalOrganisms;
                std::atomic<unsigned> processedCount;
                bool progressBarEnabled;
                std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
                
                /* progress bar display state */
                unsigned lastLineLength;
                
                /* monitor thread */
                std::thread monitorThread;
                std::atomic<bool> isComplete;

                /* render progress bar */
                void renderProgressBar(unsigned width = 50);
                
                /* progress monitor function */
                void monitorProgress(void*);

            public:

                /* adds the given vector of Organisms */
                void add(vector<shared_ptr<Organism>> organisms);

                /* wait for all workers */
                void join();

                /* start the worker of this */
                void start();

                /* enable/disable progress bar display */
                void enableProgressBar(bool enable = true) { progressBarEnabled = enable; }

                /* reset the progress counter */
                void resetProgress();
                
                /* clear the progress bar from the terminal */
                void clearProgressBar();

                /* constructor */
                ParallelProcessor(const unsigned numThreads, FitnessCalculator *fitnessCalculator);

        };
};

#endif /* __PARALLEL_WORKER_H__ */
