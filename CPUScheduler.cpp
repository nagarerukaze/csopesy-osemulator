#include "CPUScheduler.h"
#include <vector>
#include <thread>
#include <iostream>

CPUScheduler::CPUScheduler() {}
CPUScheduler::CPUScheduler(const CPUScheduler&) {}

CPUScheduler* CPUScheduler::sharedInstance = nullptr;

void CPUScheduler::initialize() {
	sharedInstance = new CPUScheduler();
}

CPUScheduler* CPUScheduler::getInstance() {
	return sharedInstance;
}

void CPUScheduler::startWorkerThreads() {
    // Create and start 4 worker threads (one per core)
    for (int i = 0; i < 4; ++i) {
        // Create and add each thread to the workerThreads vector
        workerThreads.emplace_back([this, i]() { workerFunction(i); });
    }

    // Detach all threads so they run independently
    for (auto& thread : workerThreads) {
        if (thread.joinable()) {
            thread.detach();  // Detach each thread to allow independent execution
        }
    }
}

void CPUScheduler::workerFunction(int coreID) {
    // Run while the atomic flag 'running' is set to true
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(17));  // Simulate work
        // std::cout << "CPU Core " << coreID << " is executing tasks.\n";
    }
}

int CPUScheduler::assignCPUWorker() {
	return 0;
}

