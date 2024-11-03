#include "CPUScheduler.h"
#include "ProcessManager.h"

CPUScheduler::CPUScheduler(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec) {
    this->scheduler = scheduler;
    this->numberOfCores = num_cpu;
    this->quantum_cycles = quantum_cycles;
    this->delay_per_exec = delay_per_exec;
    this->cpuCycles = 0;
}

CPUScheduler::CPUScheduler(const CPUScheduler&) {}

CPUScheduler* CPUScheduler::sharedInstance = nullptr;

void CPUScheduler::initialize(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec) {
	sharedInstance = new CPUScheduler(scheduler, num_cpu, quantum_cycles, delay_per_exec);
    sharedInstance->initializeCPUWorkers(sharedInstance->numberOfCores);
    
    // For Testing
    //std::cout << "quantum cycles: " << quantum_cycles;
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
    std::lock_guard<std::mutex> lock(mtx);
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

void CPUScheduler::FCFSScheduling() {
    while(running) {
        if(!processQueue.empty() || this->getNumberOfCPUsUsed() > 0) {
            for(int i = 0; i < this->numberOfCores; i++) {
                CPUWorker* worker = this->cpuWorkers[i];

                // when worker is finished with all instructions
                if (worker->hasProcess() && (worker->getProcess()->getCurrentInstructionLine() == worker->getProcess()->getTotalLinesOfCode())) {
                    worker->getProcess()->setState(Process::ProcessState::TERMINATED);
                    ProcessManager::getInstance()->moveToFinished(worker->getProcess()->getName());
                    worker->setProcess(nullptr);
                }

                // When worker is idle and processqueue is not empty
                if(!worker->hasProcess() && !processQueue.empty()) {
                   
                    Process* process = processQueue.front();
                    processQueue.pop();
                    
                    cpuWorkers[i]->setProcess(process);
                    std::thread thread([worker = cpuWorkers[i]]() { worker->startWorker(); });
                    thread.detach(); // Detach the thread
                }
                
            }
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(this->delay_per_exec));
    }
}

void CPUScheduler::RRScheduling() {
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
                                ProcessManager::getInstance()->moveToFinished(process_out->getName());
                            }
                        }
                        process_in = processQueue.front();
                        processQueue.pop();
                        worker->setProcess(process_in);
                    }

                    std::thread(&CPUWorker::startWorker, worker).detach();
                    
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
                        ProcessManager::getInstance()->moveToFinished(process_out->getName());
                        worker->setProcess(nullptr);
                    }
                }
            }

        }

        this->cpuCycles++;
        if (this->cpuCycles > this->quantum_cycles) {
            this->cpuCycles = 1;  // Reset the cycle counter
        }
        
    }
}

// FOR TESTING
/*
CPUWorker* CPUScheduler::getCPUWorker(int id) {
    return cpuWorkers[id];
}
*/