/**
 * simple linux and windows thread library
 */
#ifndef __THREADS_H__
#define __THREADS_H__

#ifdef _WIN32
#include <windows.h>
#include <atomic>
#else
#include <pthread.h>
#endif

#include <inttypes.h>

namespace NEAT {

    /* abstract thread class to override */
    class Thread {
        
        private:

            /* thread function to override by child class */
            virtual void threadFunction(void *args) = 0;

            /* the actual thread function */
#ifdef _WIN32
            static void startThread(void *args);
#else
            static void *startThread(void *args);
#endif

            /* the custom thread args */
            void *args;

            /* thread handle */
#ifdef _WIN32
            HANDLE thread;
#else
            pthread_t thread;
#endif

        public:

            /* start thread execution */
            void run(void *args = NULL);

            /* wait for thread execution to finish */
            void join();
            
            /* return an unqie thread identifier */
            static uint64_t id();

            /* handles execution to an other thread */
            static void yield();

            /* constructor */
            Thread();

            /* destructor */
            virtual ~Thread() { }

    };

    /* mutex class */
    class Mutex {
        
        private:

            /* the mutex object of this */
#ifdef _WIN32
            std::atomic_flag mutex = ATOMIC_FLAG_INIT;
#else
            pthread_mutex_t mutex;
#endif

        public:
            
            /* create a new Mutex */
            Mutex();

            /* delete a Mutex */
            ~Mutex();

            /* lock the mutex */
            void lock();

            /* try locking the mutex */
            bool trylock();

            /* unlock the mutex */
            void unlock();
    };

} 
    
#endif
