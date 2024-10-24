#include "CPUScheduler.h"

CPUScheduler::CPUScheduler() {}
CPUScheduler::CPUScheduler(const CPUScheduler&) {}

CPUScheduler* CPUScheduler::sharedInstance = nullptr;

void CPUScheduler::initialize(String scheduler, int num_cpu, int quantum_cycles, int delay_per_exec) {
	sharedInstance = new CPUScheduler();
    
    this->scheduler = scheduler;
    this->numberOfCores = num_cpu;
    this->quantum_cycles = quantum_cycles;
    this->delay_per_exec = delay_per_exec;
}

void CPUScheduler::initializeCPUWorkers(int num) {
    this->numberOfCores = num;
    for (int i = 0; i < num; i++) {
        CPUWorker* worker = new CPUWorker(i, this->delay_per_exec);
        this->cpuWorkers.push_back(worker);
    }
}

CPUScheduler* CPUScheduler::getInstance() {
	return sharedInstance;
}

void CPUScheduler::enqueueProcess(Process* process) {
    processQueue.push(process);
    // std::cout << "Enqueued process: " << process->getName() << std::endl; // remove later
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

                //std::cout << "Core " << i << " is executing process: " << process->getName() << process->getTimestamp() << std::endl;
            }
            else {
                // If no process is available, sleep for a while before checking again
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
}

void CPUScheduler::FCFSScheduling() {
    while(running) {
        if(!processQueue.empty()) {
            for(int i = 0; i < this->numberOfCores; i++) {
                if(!cpuWorkers[i]->hasProcess()) {
                    Process* process = processQueue.front();
                    processQueue.pop();
                    
                    cpuWorkers[i]->setProcess(process);
                    std::thread thread([worker = cpuWorkers[i]]() { worker->startWorker(); });
                    thread.detach(); // Detach the thread
                }
            }
        }

        this->cpuCycles++;
        std::this_thread::sleep_for(std::chrono::milliseconds(this->delay_per_exec));
    }
}

void CPUScheduler::RRScheduling() {
    while(running) {
        for(int i = 0; i < this->numberOfCores; i++) {
            if(!processQueue.empty()) {
                Process* process_in = processQueue.front();
                processQueue.pop();

                if(cpuWorkers[i]->hasProcess()) {
                    Process* process_out = cpuWorkers[i]->getProcess();

                    if(process_out->getState() != Process::TERMINATED) {
                        processQueue.push(process_out);
                    }
                }
                
                cpuWorkers[i]->setProcess(process_in);
                std::thread thread([worker = cpuWorkers[i]]() { worker->startWorker(); });
                thread.detach(); // Detach the thread
            }
        }

        this->cpuCycles++;
        std::this_thread::sleep_for(std::chrono::milliseconds(this->quantum_cycles));   
    }
}
