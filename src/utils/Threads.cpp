#include "Threads.h"
using namespace NEAT;

#ifdef _WIN32
#include <process.h>
#endif


#ifdef _WIN32
void Thread::startThread(void *args) {
#else
void *Thread::startThread(void *args) {
#endif
    
    Thread *self = (Thread *) args;
    self->threadFunction(self->args);

#ifdef _WIN32
    _endthread();
#else
    return NULL;
#endif
}

void Thread::run(void *args) {
    this->args = args;

#ifdef _WIN32
    thread = (HANDLE) _beginthread(startThread, 0, (void *) this);
#else
    pthread_create(&thread, NULL, startThread, (void *) this);
#endif
}

void Thread::join() {
#ifdef _WIN32
    WaitForSingleObject(thread, INFINITE);
#else
    pthread_join(thread, NULL);
#endif
}

/* create a new Mutex */
Mutex::Mutex() {
#ifndef _WIN32
    pthread_mutex_init(&mutex, NULL);
#endif
}

/* delete a Mutex */
Mutex::~Mutex() {
#ifndef _WIN32
    pthread_mutex_destroy(&mutex);
#endif
}

/* lock the mutex */
void Mutex::lock() {
#ifdef _WIN32
    while (mutex.test_and_set());
#else
    pthread_mutex_lock(&mutex);
#endif
}

/* unlock the mutex */
void Mutex::unlock() {
#ifdef _WIN32
    mutex.clear();
#else
    pthread_mutex_unlock(&mutex);
#endif
}

/* constructor */
Thread::Thread() : args(NULL) {
#ifdef _WIN32
    thread = nullptr;
#else
    thread = pthread_t();
#endif
}
