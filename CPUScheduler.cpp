#include "CPUScheduler.h"
#include "ProcessManager.h"

CPUScheduler::CPUScheduler() {}

CPUScheduler::CPUScheduler(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec, String allocator) {
    this->scheduler = scheduler;
    this->numberOfCores = num_cpu;
    this->quantum_cycles = quantum_cycles;
    this->delay_per_exec = delay_per_exec;
    this->allocator = allocator;
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

                // TODO: Paging Allocator
                
            }

            worker->removeProcess();  // Remove process from worker
        }
    }
}

bool CPUScheduler::getIsRunning() {
    return this->running;
}

int CPUScheduler::getNumberOfCPUsUsed() {
    int cpus_used = 0;

    for (int i = 0; i < this->numberOfCores; i++)
    {
        if(cpuWorkers[i]->getProcess() != nullptr && cpuWorkers[i]->getProcess()->getState() == Process::ProcessState::RUNNING) {
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
    while (this->running) {
        if (!processQueue.empty()) {
            for (int i = 0; i < this->numberOfCores; i++) {
                CPUWorker* worker = cpuWorkers[i];
                
                // Check if worker is running
                if (!worker->isRunning()) {
                    std::shared_ptr<Process> process = nullptr;
                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        // if worker has process and worker is not running anymore
                        if (worker->getProcess() != nullptr) {
                            process = worker->getProcess();
                            handleProcessOut(worker, process);
                        }
                    }

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
                                //std::cout << "Before try to allocate:" << std::endl;
                                //PagingAllocator::getInstance()->visualizeMemory();
                                allocatedMemory = PagingAllocator::getInstance()->allocate(process);

                                //std::cout << "After try to allocate:" << std::endl;
                                //PagingAllocator::getInstance()->visualizeMemory();
                            }

                            if (allocatedMemory != nullptr) {
                                process->setMemoryPointer(allocatedMemory);
                                worker->setProcess(process);
                                std::thread([worker, process = process]() {
                                    worker->setProcess(process);
                                    worker->startWorker();
                                    }).detach();
                            }
                        }
                    }
                }
            }
        }
        else {
            // Go through CPU workers and handle process out
            for (int i = 0; i < this->numberOfCores; i++) {
                CPUWorker* worker = cpuWorkers[i];
                if (worker->getProcess() != nullptr && !worker->isRunning()) {
                    std::lock_guard<std::mutex> lock(mtx);
                    std::shared_ptr<Process> process = worker->getProcess();
                    handleProcessOut(worker, process);
                }
            }
        }
        this->cpuCycles++;
    }
}

void CPUScheduler::handleProcessOut(CPUWorker* worker, std::shared_ptr<Process>& process_out) {
    if (process_out != nullptr) {
        if (process_out->getCurrentInstructionLine() < process_out->getTotalLinesOfCode()) {
            // Not done executing, requeue the process
            worker->removeProcess();
            processQueue.push(process_out);
        }
        else {
            // Done executing, remove process from worker
            worker->removeProcess();
        }
    }
}

void CPUScheduler::RRScheduling() {
    while (this->running) {
        if (!processQueue.empty()) {
            for (int i = 0; i < this->numberOfCores; i++) {
                CPUWorker* worker = cpuWorkers[i];
                std::shared_ptr<Process> process_out = nullptr;
                std::shared_ptr<Process> process_in = nullptr;
                void* allocatedMemory = nullptr;

                // has a process queue
                if (!worker->isRunning()) {
                    // std::cout << "There is a ready queue and the worker is not running" << std::endl;

                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        // if worker has process and worker is not running anymore
                        if (worker->getProcess() != nullptr) {
                            process_out = worker->getProcess();
                            handleProcessOut(worker, process_out);
                        }
                    }

                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        if (!processQueue.empty()) {
                            // get the process and allocated memory
                            process_in = processQueue.front();
                            processQueue.pop();

                            if (!process_in) {
                                // std::cerr << "Error: process_in is nullptr\n";
                                continue; // Skip to the next iteration
                            }

                            allocatedMemory = process_in->getMemoryPointer();

                            // if process is not allocated in memory
                            if (allocatedMemory == nullptr) {
                                if (allocator == "flat") {
                                    MemoryManager::getInstance()->removeProcessFromBS(process_in->getName());
                                    allocatedMemory = MemoryManager::getInstance()->allocate(process_in->getMemoryRequired(), process_in->getName());
                                }
                                else {
                                    PagingAllocator::getInstance()->removeProcessFromBS(process_in->getName());
                                    allocatedMemory = PagingAllocator::getInstance()->allocate(process_in);
                                }

                                // allocatation failed, TODO: put in backing store
                                if (allocatedMemory == nullptr) {
                                    String name = "";
                                    std::shared_ptr<Process> oldest_process = nullptr;
                                    if (allocator == "flat") {
                                        name = MemoryManager::getInstance()->removeOldestEntry();
                                        oldest_process = ProcessManager::getInstance()->findProcess(name);
                                        MemoryManager::getInstance()->saveProcessToBS(oldest_process->getMemoryPointer(), oldest_process->getMemoryRequired(), oldest_process->getName());
                                        allocatedMemory = MemoryManager::getInstance()->allocate(process_in->getMemoryRequired(), process_in->getName());
                                    }
                                    else {
                                        name = PagingAllocator::getInstance()->removeOldestEntry();
                                        PagingAllocator::getInstance()->saveProcessToBS(name);
                                        allocatedMemory = PagingAllocator::getInstance()->allocate(process_in);
                                        oldest_process = ProcessManager::getInstance()->findProcess(name);

                                    }
                                    // Find oldest process and push back in queue
                                    process_in->setMemoryPointer(allocatedMemory);
                                    processQueue.push(oldest_process);
                                }
                                // set pointer if allocated
                                else {
                                    process_in->setMemoryPointer(allocatedMemory);
                                }
                            }
                            if (allocatedMemory != nullptr) {
                                worker->setProcess(process_in);  // Ensure that process_in is valid
                                std::thread([worker, process_in = process_in]() {
                                    worker->setProcess(process_in);  // Ensure that process_in is valid
                                    worker->startWorker(); // Start worker with the shared_ptr process
                                    }).detach();
                            }
                        }
                    }
                }
                // std::cout << "Memory" << std::endl << MemoryManager::getInstance()->visualizeMemory() << std::endl;
            }
        }
        // no ready queue
        else {
            // Go through CPU workers and handle process out
            for (int i = 0; i < this->numberOfCores; i++) {
                CPUWorker* worker = cpuWorkers[i];
                if (worker->getProcess() != nullptr && !worker->isRunning()) {
                    std::lock_guard<std::mutex> lock(mtx);
                    std::shared_ptr<Process> process_out = worker->getProcess();
                    handleProcessOut(worker, process_out);
                }
            }
        }
        this->cpuCycles++;
    }
}
