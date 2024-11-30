#include "CPUScheduler.h"
#include "ProcessManager.h"

CPUScheduler::CPUScheduler() {}

CPUScheduler::CPUScheduler(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec, String allocator) {
    this->scheduler = scheduler;
    this->numberOfCores = num_cpu;
    this->quantum_cycles = quantum_cycles;
    this->delay_per_exec = delay_per_exec;
    this->allocator = allocator;
    this->idleCPUTicks = 0;
    this->activeCores = 0;
    this->activeCPUTicks = 0;
}

CPUScheduler::CPUScheduler(const CPUScheduler&) {}

CPUScheduler* CPUScheduler::sharedInstance = nullptr;

std::atomic<long long> CPUScheduler::cpuCycles{ 0 };

void CPUScheduler::initialize(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec, String allocator) {
	sharedInstance = new CPUScheduler(scheduler, num_cpu, quantum_cycles, delay_per_exec, allocator);
    sharedInstance->initializeCPUWorkers(sharedInstance->numberOfCores);
}

void CPUScheduler::initializeCPUWorkers(int num) {
    this->numberOfCores = num;
    for (int i = 0; i < num; i++) {
        CPUWorker* worker = new CPUWorker(i, this->delay_per_exec, this->scheduler, this->quantum_cycles, this->allocator);
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
        if (worker->getProcess() != nullptr) {
            auto process = worker->getProcess();  // Get shared_ptr to the current process

            if (process && process->getCurrentInstructionLine() < process->getTotalLinesOfCode()) {
                // Mark process as terminated
                process->setState(Process::ProcessState::TERMINATED);
                process->setCPUCoreID(NULL);

                // Flat Memory Allocator
                if (allocator == "flat") {
                    MemoryManager::getInstance()->deallocate(process->getMemoryPointer(), process->getMemoryRequired(), process->getName());
                    process->setMemoryPointer(nullptr);
                }
                else {
                    PagingAllocator::getInstance()->deallocate(process->getName());
                }
                
            }

            worker->removeProcess();  // Remove process from worker
        }
    }

    this->runningProcesses.clear();
    this->cpuWorkers.clear();
}

bool CPUScheduler::getIsRunning() {
    return this->running;
}

int CPUScheduler::getNumberOfCPUsUsed() {
    return this->activeCores;
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
    while (this->running) {
        if (!processQueue.empty()) {
            for (int i = 0; i < this->numberOfCores; i++) {
                CPUWorker* worker = cpuWorkers[i];
                
                // Check if worker is running
                if (!worker->isRunning()) {
                    std::shared_ptr<Process> process = nullptr;
                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        if (!processQueue.empty()) {
                            process = processQueue.front();
                            processQueue.pop();
                            void* allocatedMemory = nullptr;

                            if (allocator == "flat") {
                                allocatedMemory = MemoryManager::getInstance()->allocate(process->getMemoryRequired(), process->getName());
                            }
                            // Paging
                            else {
                                allocatedMemory = PagingAllocator::getInstance()->allocate(process);
                            }

                            if (allocatedMemory != nullptr) {
                                process->setMemoryPointer(allocatedMemory);
                                worker->setProcess(process);
                                this->runningProcesses[i] = process;
                                incrementCores();
                                std::thread([worker, process = process]() {
                                    worker->setProcess(process);
                                    worker->startWorker();
                                    }).detach();
                            }
                        }
                    }
                }
                else {
                    this->idleCPUTicks++;
                }
            }
        }
        this->cpuCycles++;
    }
}

void CPUScheduler::RRScheduling() {
    while (this->running) {
        if (!processQueue.empty()) {
            for (int i = 0; i < this->numberOfCores; i++) {
                CPUWorker* worker = cpuWorkers[i];
                std::shared_ptr<Process> process_in = nullptr;
                void* allocatedMemory = nullptr;

                // has a process queue
                if (!worker->isRunning() && worker->getProcess() == nullptr) {
                    if (!processQueue.empty()) {
                        {
                            std::lock_guard<std::mutex> lock(mtx);
                            process_in = processQueue.front();
                            processQueue.pop();

                            if (!process_in) {
                                continue; // Skip to the next iteration
                            }
                            allocatedMemory = process_in->getMemoryPointer();
                        }

                        if (allocatedMemory == nullptr) {
                            if (allocator == "flat") {
                                MemoryManager::getInstance()->removeProcessFromBS(process_in->getName());
                                allocatedMemory = MemoryManager::getInstance()->allocate(process_in->getMemoryRequired(), process_in->getName());
                            }
                            else {
                                PagingAllocator::getInstance()->removeProcessFromBS(process_in->getName());
                                allocatedMemory = PagingAllocator::getInstance()->allocate(process_in);
                            }

                            if (allocatedMemory == nullptr) {
                                std::shared_ptr<Process> oldest_process = nullptr;
                                if (allocator == "flat") {
                                    oldest_process = MemoryManager::getInstance()->removeOldestEntry(); // Remove oldest process in memory

                                    MemoryManager::getInstance()->deallocate(oldest_process->getMemoryPointer(), oldest_process->getMemoryRequired(), oldest_process->getName());
                                    MemoryManager::getInstance()->saveProcessToBS(oldest_process->getMemoryPointer(), 
                                                                                oldest_process->getMemoryRequired(), 
                                                                                oldest_process->getName()); // Save process to backing store
                                    allocatedMemory = MemoryManager::getInstance()->allocate(process_in->getMemoryRequired(), process_in->getName());
                                }
                                else {
                                    oldest_process = PagingAllocator::getInstance()->removeOldestEntry();
                                    if (oldest_process != nullptr) {
                                        PagingAllocator::getInstance()->deallocate(oldest_process->getName());
                                        PagingAllocator::getInstance()->saveProcessToBS(oldest_process->getName());
                                        allocatedMemory = PagingAllocator::getInstance()->allocate(process_in);
                                    }

                                }
                                if (oldest_process != nullptr) {
                                    oldest_process->setMemoryPointer(nullptr);
                                    process_in->setMemoryPointer(allocatedMemory);
                                    processQueue.push(oldest_process);
                                }
                                else {
                                    processQueue.push(process_in);
                                }
                            }
                            // set pointer if allocated
                            else {
                                process_in->setMemoryPointer(allocatedMemory);
                            }
                        }
                        {
                            std::lock_guard<std::mutex> lock(mtx);
                            if (allocatedMemory != nullptr) {
                                worker->setProcess(process_in);
                                // std::cout << "process_in" << process_in->getName() << std::endl;
                                this->runningProcesses[i] = process_in;
                                incrementCores();
                                std::thread([worker, process_in = process_in]() {
                                    worker->setProcess(process_in);
                                    worker->startWorker();
                                    }).detach();
                            }
                        }
                    }
                }
                else {
                    this->idleCPUTicks++;
                }
            }
        }
        this->cpuCycles++;
    }
}

size_t CPUScheduler::getIdleCPUTicks() const {
    return this->idleCPUTicks;
}

size_t CPUScheduler::getActiveCPUTicks() const {
    return this->activeCPUTicks;
}

String CPUScheduler::getAllocator() const {
    return this->allocator;
}

void CPUScheduler::decrementCores() {
    this->activeCores--;
}

void CPUScheduler::incrementCores() {
    this->activeCores++;
}

void CPUScheduler::removeFromRunning(int index) {
    this->runningProcesses.erase(index);
}

std::unordered_map<int, std::shared_ptr<Process>> CPUScheduler::getRunningProcesses() {
    return this->runningProcesses;
}

