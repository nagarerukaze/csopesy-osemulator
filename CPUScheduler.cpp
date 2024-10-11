#include "CPUScheduler.h"

CPUScheduler::CPUScheduler() {}
CPUScheduler::CPUScheduler(const CPUScheduler&) {}

CPUScheduler* CPUScheduler::sharedInstance = nullptr;

void CPUScheduler::initialize() {
	sharedInstance = new CPUScheduler();
}

void CPUScheduler::initializeCPUWorkers(int num) {
    this->numberOfCores = num;
    for (int i = 0; i < num; i++) {
        CPUWorker* worker = new CPUWorker(i);
        this->cpuWorkers.push_back(worker);
    }
}

CPUScheduler* CPUScheduler::getInstance() {
	return sharedInstance;
}

void CPUScheduler::enqueueProcess(Process* process) {
    processQueue.push(process);
    std::cout << "Enqueued process: " << process->getName() << std::endl; // remove later
}

void CPUScheduler::startScheduler() {
    int i = 0;
    while(running) {
        if(cpuWorkers[i]->hasProcess()) {
            i = (i + 1) % 4;
        }
        else {
            if (!processQueue.empty()) {
                Process* process = processQueue.front(); // Get the next process
                processQueue.pop(); // Remove it from the queue

                cpuWorkers[i]->setProcess(process);
                std::thread thread([worker = cpuWorkers[i]]() { worker->startWorker(); });
                thread.detach(); // Detach the thread

                std::cout << "Core " << i << " is executing process: " << process->getName() << std::endl;
                }
            else {
                // If no process is available, sleep for a while before checking again
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
}

