#include "CPUScheduler.h"
#include "ProcessManager.h"

CPUScheduler::CPUScheduler() {}

CPUScheduler::CPUScheduler(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec) {
    this->scheduler = scheduler;
    this->numberOfCores = num_cpu;
    this->quantum_cycles = quantum_cycles;
    this->delay_per_exec = delay_per_exec;
}

CPUScheduler::CPUScheduler(const CPUScheduler&) {}

CPUScheduler* CPUScheduler::sharedInstance = nullptr;

std::atomic<long long> CPUScheduler::cpuCycles{ 0 };

void CPUScheduler::initialize(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec) {
	sharedInstance = new CPUScheduler(scheduler, num_cpu, quantum_cycles, delay_per_exec);
    sharedInstance->initializeCPUWorkers(sharedInstance->numberOfCores);
}

void CPUScheduler::initializeCPUWorkers(int num) {
    this->numberOfCores = num;
    for (int i = 0; i < num; i++) {
        CPUWorker* worker = new CPUWorker(i, this->delay_per_exec);
        this->cpuWorkers.push_back(worker);
    }
}

CPUScheduler* CPUScheduler::getInstance() {
    if (sharedInstance == NULL)
    {
        sharedInstance = new CPUScheduler;
    }

    return sharedInstance;
}

void CPUScheduler::enqueueProcess(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(mtx);
    //std::cout << "Enqueuing process: " << process->getName() << std::endl;
    this->processQueue.push(process);
}


void CPUScheduler::startScheduler() {
    this->running = true;
    
    if (this->scheduler == "fcfs") {
        FCFSScheduling();
    }
    else if (this->scheduler == "rr") {
        RRScheduling();
    }
}

void CPUScheduler::stopScheduler() {
    // Set the running flag to false, which will stop the scheduler's main loop
    this->running = false;

    // Clean up CPU workers by checking their process states
    for (auto& worker : cpuWorkers) {
        // If the worker has a process assigned
        if (worker->hasProcess()) {
            auto process = worker->getProcess();  // Get shared_ptr to the current process

            if (process && process->getCurrentInstructionLine() < process->getTotalLinesOfCode()) {
                // Mark process as terminated
                process->setState(Process::ProcessState::TERMINATED);
                ProcessManager::getInstance()->moveToFinished(process);  // Move process to finished state
            }

            worker->setProcess(nullptr);  // Remove process from worker
        }
    }
}

int CPUScheduler::getNumberOfCPUsUsed() {
    int cpus_used = 0;

    for (int i = 0; i < this->numberOfCores; i++)
    {
        if(cpuWorkers[i]->hasProcess()) {
           cpus_used++; 
        }
    }

    return cpus_used;
}

int CPUScheduler::getNumberOfCores() {
    return this->numberOfCores;
}

std::vector<CPUWorker*> CPUScheduler::getCPUWorkers() {
    return this->cpuWorkers;
}

long long CPUScheduler::getQuantumCycles() {
    return this->quantum_cycles;
}

void CPUScheduler::FCFSScheduling() {
    while (running) {
        // std::cout << "SCHEDULER CYCLE IN FCFS: " << CPUScheduler::cpuCycles << std::endl;
        if (!processQueue.empty() || this->getNumberOfCPUsUsed() > 0) {
            for (int i = 0; i < this->numberOfCores; i++) {
                CPUWorker* worker = this->cpuWorkers[i];

                // When worker is finished with all instructions
                if (worker->hasProcess() &&
                    (worker->getProcess()->getCurrentInstructionLine() == worker->getProcess()->getTotalLinesOfCode())) {
                    worker->getProcess()->setState(Process::ProcessState::TERMINATED);
                    ProcessManager::getInstance()->moveToFinished(worker->getProcess());
                    worker->setProcess(nullptr);  // Clear process after termination
                }

                // When worker is idle and processQueue is not empty
                if (!worker->hasProcess() && !processQueue.empty()) {
                    std::lock_guard<std::mutex> lock(mtx); // Protect access to the queue
                    std::shared_ptr<Process> process = processQueue.front();  // Get shared_ptr from the queue
                    processQueue.pop();  // Remove the process from the queue

                    worker->setProcess(process);  // Set the shared_ptr in the worker
                    std::thread([worker, process]() {
                        worker->startWorker(); // Start worker with the shared_ptr process
                        }).detach();  // Detach the thread to run concurrently
                }
            }
        }
        this->cpuCycles++;
    }
}

//void CPUScheduler::FCFSScheduling() {
    //    while(running) {
    //        if(!processQueue.empty() || this->getNumberOfCPUsUsed() > 0) {
    //            for(int i = 0; i < this->numberOfCores; i++) {
    //                CPUWorker* worker = this->cpuWorkers[i];
    //
    //                // when worker is finished with all instructions
    //                if (worker->hasProcess() && (worker->getProcess()->getCurrentInstructionLine() == worker->getProcess()->getTotalLinesOfCode())) {
    //                    worker->getProcess()->setState(Process::ProcessState::TERMINATED);
    //                    ProcessManager::getInstance()->moveToFinished(worker->getProcess());
    //                    worker->setProcess(nullptr);
    //                }
    //
    //                // When worker is idle and processqueue is not empty
    //                if(!worker->hasProcess() && !processQueue.empty()) {
    //                   std::lock_guard<std::mutex> lock(mtx);
    //                    Process* process = processQueue.front();
    //                    processQueue.pop();
    //                    
    //                    cpuWorkers[i]->setProcess(process);
    //                    std::thread thread([worker = cpuWorkers[i]]() { worker->startWorker(); });
    //                    thread.detach(); // Detach the thread
    //                }
    //                
    //            }
    //        }
    //    }
    //}
    //
//}

void CPUScheduler::RRScheduling() {
    /*
    while (running) {
        // Only switch processes at the start of each quantum cycle
        if (this->cpuCycles == 1) { 
            for (int i = 0; i < this->numberOfCores; i++) {
                CPUWorker* worker = cpuWorkers[i];
                Process* process_out = nullptr;
                Process* process_in = nullptr;
                
                if (!processQueue.empty()) {
                    // Safely remove the current process and requeue it if needed
                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        if (worker->hasProcess()) {
                            process_out = worker->getProcess();
                            if (process_out != nullptr && process_out->getCurrentInstructionLine() < process_out->getTotalLinesOfCode()) {
                                processQueue.push(process_out);
                                process_out->setState(Process::ProcessState::READY);
                            }
                            else {
                                process_out->setState(Process::ProcessState::TERMINATED);
                                ProcessManager::getInstance()->moveToFinished(process_out);
                            }
                        }
                        process_in = processQueue.front();
                        processQueue.pop();
                        worker->setProcess(process_in);
                    }

                    std::thread(&CPUWorker::startWorker, worker).detach();
                    
                    generateMemorySnapshot(this->cpuCycles);
                    // For Testing
                    //std::cout<<"CPU #"<< i+1 << ": " << worker->getProcess()->getName() << std::endl;
                }
                else if (worker->hasProcess()) {
                    // handle the case where the queue is empty but the last 4 processes haven't been terminated properly
                    process_out = worker->getProcess();
                    if (process_out != nullptr && process_out->getCurrentInstructionLine() < process_out->getTotalLinesOfCode()) {
                        processQueue.push(process_out);
                        process_out->setState(Process::ProcessState::READY);
                    }
                    else {
                        process_out->setState(Process::ProcessState::TERMINATED);
                        ProcessManager::getInstance()->moveToFinished(process_out);
                        worker->setProcess(nullptr);
                    }

                    generateMemorySnapshot(this->cpuCycles);
                }
            }

        }

        this->cpuCycles++;
        if (this->cpuCycles > this->quantum_cycles) {
            this->cpuCycles = 1;  // Reset the cycle counter
        }
        
    }
    */
}

