#include "CPUScheduler.h"
#include "Process.h"
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

void CPUScheduler::enqueueProcess(Process* process) {
    processQueue.push(process);

    // Create a temporary queue to iterate over
    std::queue<Process*> tempQueue = processQueue;

    // Print the names of processes currently in the queue
    std::cout << "Processes in queue: ";
    while (!tempQueue.empty()) {
        std::cout << tempQueue.front()->getName() << " "; // Get the name of the process
        tempQueue.pop(); // Remove it from the temporary queue
    }
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

