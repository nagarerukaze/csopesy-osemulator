#include "CPUScheduler.h"

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
    std::cout << "Enqueued process: " << process->getName() << std::endl; // remove later
}

void CPUScheduler::startWorkerThreads() {
    // Create and start 4 worker threads (one per core)
    for (int i = 1; i <= 4; i++) {
        std::thread workerThread(&CPUScheduler::workerFunction, this, i);
        workerThreads.emplace_back(std::move(workerThread));
    }

    // Detach all threads so they run independently
    for (auto& thread : workerThreads) {
        if (thread.joinable()) {
            thread.detach();
        }
    }
}

// TODO: This is not doing first come first serve
void CPUScheduler::workerFunction(int coreID) {
    while (running) {
        Process* process = nullptr;

        // Check if there are any processes in the queue
        if (!processQueue.empty()) {
            process = processQueue.front(); // Get the next process
            processQueue.pop(); // Remove it from the queue

            std::cout << "Core " << coreID << " is executing process: " << process->getName() << std::endl;
            // process->executePrintCommands(coreID); // Execute its print commands

            // TODO: set process to finished / add to finishedprocesses
        }
        else {
            // If no process is available, sleep for a while before checking again
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

int CPUScheduler::assignCPUWorker() {
	return 0;
}

