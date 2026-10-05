#include "ParallelWorker.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

using namespace NEAT;

/* thread function to override by child class */
void ParallelWorker::threadFunction(void *) {
    while (!this->organisms.empty()) {
        shared_ptr<Organism> organism;
        mutex.lock();

        if (!this->organisms.empty()) {
            organism = this->organisms.front();
            this->organisms.pop_front();
        }

        mutex.unlock();

        if (organism != nullptr) {
            organism->calculateFittness(fitnessCalculator);
            // Increment the processed count for progress tracking
            ++processedCount;
        }
    }
}

/* render the progress bar */
void ParallelProcessor::renderProgressBar(unsigned width) {
    if (!progressBarEnabled || totalOrganisms == 0) {
        return;
    }

    // Calculate percentage
    unsigned percent = (processedCount * 100) / totalOrganisms;
    
    // Calculate elapsed time
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() / 1000.0;
    
    // Calculate speed (organisms/second)
    double speed = (duration > 0) ? (processedCount / duration) : 0;
    
    // Calculate ETA
    double eta = (speed > 0) ? (totalOrganisms - processedCount) / speed : 0;
    
    // Render the progress bar - first clear the line
    std::cout << "\r";
    
    // Create buffer to ensure we overwrite the entire previous line
    std::cout << std::string(lastLineLength, ' ');
    
    // Go back to start of line
    std::cout << "\r[";
    
    unsigned pos = width * percent / 100;
    for (unsigned i = 0; i < width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    
    std::stringstream ss;
    ss << "] " << std::setw(3) << percent << "% "
       << processedCount << "/" << totalOrganisms
       << " [" << std::fixed << std::setprecision(1) << speed << " org/s, "
       << std::setprecision(1) << duration << "s elapsed, ETA: " 
       << std::setprecision(1) << eta << "s]";
    
    std::cout << ss.str();
    
    // Store the line length for future clearing
    lastLineLength = ss.str().length() + width + 3; // +3 for "[" and spaces
    
    std::cout.flush();
}

/* clear the progress bar */
void ParallelProcessor::clearProgressBar() {
    if (!progressBarEnabled) return;
    
    // Go to beginning of line and clear it
    std::cout << "\r" << std::string(lastLineLength, ' ') << "\r";
    std::cout.flush();
}

/* Progress monitor thread function */
void ParallelProcessor::monitorProgress(void*) {
    while (!isComplete && progressBarEnabled) {
        renderProgressBar();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Update 10 times per second
    }
}

/* reset progress */
void ParallelProcessor::resetProgress() {
    processedCount = 0;
    lastPrintedPercent = 0;
    startTime = std::chrono::high_resolution_clock::now();
    isComplete = false;
}

/* adds the given vector of Organisms */
void ParallelProcessor::add(vector<shared_ptr<Organism>> organisms) {
    mutex.lock();
    this->organisms.insert(this->organisms.end(), organisms.begin(), organisms.end());
    
    // Update total count for progress tracking
    totalOrganisms += organisms.size();
    
    mutex.unlock();
}

/* wait for all workers */
void ParallelProcessor::join() {
    for (auto &w : worker) {
        w.join();
    }
    
    // Signal monitor thread to stop
    isComplete = true;
    
    if (monitorThread.joinable()) {
        monitorThread.join();
    }
    
    // Clear the progress bar when done
    clearProgressBar();
}

void ParallelProcessor::start() {
    // Reset progress tracking when starting
    resetProgress();
    
    // Start the progress monitor thread
    if (progressBarEnabled) {
        isComplete = false;
        monitorThread = std::thread(&ParallelProcessor::monitorProgress, this, nullptr);
    }
    
    // Start worker threads
    for (auto &w : worker) {
        w.run();
    }
}

/* constructor */
ParallelProcessor::ParallelProcessor(
    const unsigned numThreads, 
    FitnessCalculator *fitnessCalculator
) : fitnessCalculator(fitnessCalculator), 
    totalOrganisms(0), 
    processedCount(0),
    lastPrintedPercent(0),
    progressBarEnabled(true),
    lastLineLength(0),
    isComplete(false) {

    for (unsigned i = 0; i < numThreads; i++) {
        this->worker.push_back(ParallelWorker(
            organisms,
            mutex,
            fitnessCalculator,
            processedCount
        ));
    }
    
    // Initialize start time
    startTime = std::chrono::high_resolution_clock::now();
}
