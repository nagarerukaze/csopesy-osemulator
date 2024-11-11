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
        CPUWorker* worker = new CPUWorker(i, this->delay_per_exec, this->scheduler, this->quantum_cycles);
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

bool CPUScheduler::getIsRunning() {
    return this->running;
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
    //while (running) {
    //    // std::cout << "SCHEDULER CYCLE IN FCFS: " << CPUScheduler::cpuCycles << std::endl;
    //    if (!processQueue.empty() || this->getNumberOfCPUsUsed() > 0) {
    //        for (int i = 0; i < this->numberOfCores; i++) {
    //            CPUWorker* worker = this->cpuWorkers[i];

    //            // When worker is finished with all instructions
    //            if (worker->hasProcess() &&
    //                (worker->getProcess()->getCurrentInstructionLine() == worker->getProcess()->getTotalLinesOfCode())) {
    //                // worker->getProcess()->setState(Process::ProcessState::TERMINATED);
    //                ProcessManager::getInstance()->moveToFinished(worker->getProcess());
    //                worker->setProcess(nullptr);  // Clear process after termination
    //            }

    //            // When worker is idle and processQueue is not empty
    //            if (!worker->hasProcess() && !processQueue.empty()) {
    //                std::lock_guard<std::mutex> lock(mtx); // Protect access to the queue
    //                std::shared_ptr<Process> process = processQueue.front();  // Get shared_ptr from the queue
    //                processQueue.pop();  // Remove the process from the queue

    //                worker->setProcess(process);  // Set the shared_ptr in the worker
    //                std::thread([worker, process]() {
    //                    worker->startWorker(); // Start worker with the shared_ptr process
    //                    }).detach();  // Detach the thread to run concurrently
    //            }
    //        }
    //    }
    //    this->cpuCycles++;
    //}
}

void CPUScheduler::handleProcessOut(CPUWorker* worker, std::shared_ptr<Process>& process_out) {
    if (process_out != nullptr) {
        if (process_out->getCurrentInstructionLine() < process_out->getTotalLinesOfCode()) {
            // Not done executing, requeue the process
            processQueue.push(process_out);
            process_out->setState(Process::ProcessState::READY);
            process_out->setCPUCoreID(NULL);
        }
        else {
            // Done executing, deallocate memory
            ProcessManager::getInstance()->moveToFinished(process_out);
            // std::cout << "Deallocating for " << process_out->getName() << std::endl;
            MemoryManager::getInstance()->deallocate(process_out->getMemoryPointer(), process_out->getMemoryRequired());
            process_out->setMemoryPointer(nullptr);
        }
    }
}

void CPUScheduler::RRScheduling() {
    // ISSUE: If scheduler-test or any process is created (first process)
    // while this loop is iterating through the cpu workers, it may not be
    // assigned to cpu 1

    while (running) {
        for (int i = 0; i < this->numberOfCores; i++) {
            CPUWorker* worker = cpuWorkers[i];
            std::shared_ptr<Process> process_out = nullptr;
            std::shared_ptr<Process> process_in = nullptr;
            void* allocatedMemory = nullptr;

            // std::cout << "Worker " << i << " now," << std::endl;
            // has a process queue
            if (!processQueue.empty() && (!worker->isRunning())) {

                // std::cout << "Worker " << i << " not running," << std::endl;
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    // if worker has process and worker is not running anymore
                    if (worker->hasProcess()) {
                        process_out = worker->getProcess();
                        handleProcessOut(worker, process_out);
                    }
                }

                // get the process and allocated memory
                process_in = processQueue.front();
                // std::cout << "Process_in = " << process_in->getName() << std::endl;
                processQueue.pop();
                allocatedMemory = process_in->getMemoryPointer();

                // if process is not allocated in memory
                if (allocatedMemory == nullptr) {
                    // std::cout << "Not allocated in memory for process " << process_in->getName() << std::endl;
                    allocatedMemory = MemoryManager::getInstance()->allocate(process_in->getMemoryRequired());
                    // allocatation failed, requeue
                    if (allocatedMemory == nullptr) {
                        processQueue.push(process_in);
                        // std::cout << "Requeued. Not enough memory for process " << process_in->getName() << std::endl;
                    }
                    // set pointer if allocated
                    else {
                        // std::cout << "Allocated memory for process " << process_in->getName() << std::endl;
                        process_in->setMemoryPointer(allocatedMemory);
                    }
                }

                if (allocatedMemory != nullptr) {
                    // std::cout << "Has Memory" << process_in->getName() << std::endl;
                    worker->setProcess(process_in);
                    std::thread([worker, process_in]() {
                        worker->startWorker(); // Start worker with the shared_ptr process
                        }).detach();
                }
            }
            // no more queue, but worker has process
            else if (worker->hasProcess() && (!worker->isRunning())) {
                std::lock_guard<std::mutex> lock(mtx);
                process_out = worker->getProcess();
                handleProcessOut(worker, process_out);
            }

            // std::cout << "Memory" << std::endl << MemoryManager::getInstance()->visualizeMemory() << std::endl;
        }
        this->cpuCycles++;

        //////////////////////////////////////////////////

        // Every <quantum_cycle> CPU cycle, produce a text file
        if (this->cpuCycles % this->quantum_cycles == 0) {
            //MemoryManager::getInstance()->printMemory(quantumCycleCtr);
            this->quantumCycleCtr++;
        }
        
        //////////////////////////////////////////////////
    }
}

//
//void CPUScheduler::RRScheduling() {
//    while (running) {
//        for (int i = 0; i < this->numberOfCores; i++) {
//            CPUWorker* worker = cpuWorkers[i];
//            std::shared_ptr<Process> process_out = nullptr;
//            std::shared_ptr<Process> process_in = nullptr;
//            void* allocatedMemory = nullptr;
//
//            if (!worker->running.load()) {
//                //std::cout << "Entered worker is not runnign" << std::endl;
//                if (!processQueue.empty()) {
//                    std::lock_guard<std::mutex> lock(mtx);
//                    // if it has a process already
//                    process_out = worker->getProcess();
//                    handleProcessOut(worker, process_out);
//
//                    // get the process and allocate memory
//                    process_in = processQueue.front();
//                    processQueue.pop();
//                    if (process_in->getMemoryPointer() == nullptr) {
//                        //std::cout << "Allocating Memory" << std::endl;
//                        allocatedMemory = MemoryManager::getInstance()->allocate(process_in->getMemoryRequired());
//                        //std::cout << "Allocating Memory AFTER" << std::endl;
//                        if (allocatedMemory != nullptr) {
//                            //std::cout << "Allocated Memory" << std::endl;
//                            process_in->setMemoryPointer(allocatedMemory);
//                            worker->setProcess(process_in);
//                            std::thread([worker, process_in]() {
//                                worker->startWorker(); // Start worker with the shared_ptr process
//                                }).detach();  // Detach the thread to run concurrently
//                        }
//                        else {
//                            //std::cout << "No Memory Available" << std::endl;
//                            // No available memory; re-enqueue for future processing
//                            processQueue.push(process_in);
//                        }
//                        //std::cout << "Exited IF STATEMTN Allocated Memory" << std::endl;
//                    }
//                    else {
//                        // we can executethe process
//                        worker->setProcess(process_in);
//                        std::thread([worker, process_in]() {
//                            worker->startWorker(); // Start worker with the shared_ptr process
//                            }).detach();  // Detach the thread to run concurrently
//                    }
//
//                    //std::cout << "Exited IF ELSE STATEMTN Allocated Memory" << std::endl;
//                } // handle the case where the queue is empty but the last 4 processes haven't been terminated properly
//                else if (worker->hasProcess()) {
//                    // std::cout << "Worker " << i << " entered processqueue is empty" << std::endl;
//                    process_out = worker->getProcess();
//                    handleProcessOut(worker, process_out);
//                }
//
//                // std::cout << "Exited IF ELSE ELSE IF STATEMTN Allocated Memory" << std::endl;
//            }
//        }
//        this->cpuCycles++;
//        // std::cout << "Visualizing Memory" << std::endl;
//        // std::cout << MemoryManager::getInstance()->visualizeMemory() << std::endl;
//    }
//}

//// OLD CODE
//void CPUScheduler::RRScheduling() {
//    while (running) {
//        // Only switch processes at the start of each quantum cycle
//        if (this->cpuCycles == 1) {
//            for (int i = 0; i < this->numberOfCores; i++) {
//                CPUWorker* worker = cpuWorkers[i];
//                Process* process_out = nullptr;
//                Process* process_in = nullptr;
//
//                if (!processQueue.empty()) {
//                    // Safely remove the current process and requeue it if needed
//                    {
//                        std::lock_guard<std::mutex> lock(mtx);
//                        if (worker->hasProcess()) {
//                            process_out = worker->getProcess();
//                            if (process_out != nullptr && process_out->getCurrentInstructionLine() < process_out->getTotalLinesOfCode()) {
//                                processQueue.push(process_out);
//                                process_out->setState(Process::ProcessState::READY);
//                            }
//                            else {
//                                process_out->setState(Process::ProcessState::TERMINATED);
//                                ProcessManager::getInstance()->moveToFinished(process_out);
//                            }
//                        }
//                        process_in = processQueue.front();
//                        processQueue.pop();
//                        worker->setProcess(process_in);
//                    }
//
//                    std::thread(&CPUWorker::startWorker, worker).detach();
//
//                    // For Testing
//                    //std::cout<<"CPU #"<< i+1 << ": " << worker->getProcess()->getName() << std::endl;
//                }
//                else if (worker->hasProcess()) {
//                    // handle the case where the queue is empty but the last 4 processes haven't been terminated properly
//                    process_out = worker->getProcess();
//                    if (process_out != nullptr && process_out->getCurrentInstructionLine() < process_out->getTotalLinesOfCode()) {
//                        processQueue.push(process_out);
//                        process_out->setState(Process::ProcessState::READY);
//                    }
//                    else {
//                        process_out->setState(Process::ProcessState::TERMINATED);
//                        ProcessManager::getInstance()->moveToFinished(process_out);
//                        worker->setProcess(nullptr);
//                    }
//                }
//            }
//
//        }
//
//        this->cpuCycles++;
//        if (this->cpuCycles > this->quantum_cycles) {
//            this->cpuCycles = 1;  // Reset the cycle counter
//        }
//
//    }
//}
