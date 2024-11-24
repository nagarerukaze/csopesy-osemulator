#include "CPUWorker.h"
#include "ProcessManager.h"
#include "CPUScheduler.h"


CPUWorker::CPUWorker(int id, long long delay_per_exec, String scheduler, long long quantum_cycles) {
    this->id = id;
    this->delay_per_exec = delay_per_exec;
    this->running = false;
    this->scheduler = scheduler;
    this->quantum_cycles = quantum_cycles;
    this->process = nullptr;
}

// Set a CPU's process
void CPUWorker::setProcess(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(mtx);

    this->process = process;
    this->process->setCPUCoreID(this->id);
    this->running = true;
}

// Set process to nullptr
void CPUWorker::removeProcess() {
    std::lock_guard<std::mutex> lock(mtx);
    this->process = nullptr;
}

void CPUWorker::startWorker() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (this->process != nullptr) {
            //std::cout << "Inside worker: Process is not null" << std::endl;
            this->process->setState(Process::ProcessState::RUNNING);
            this->process->setCPUCoreID(this->id);
        }
    }
    this->cpuCycles = 0;
    // FCFS
    if (scheduler == "fcfs") {
        while (this->running) {
            if (this->cpuCycles == delay_per_exec) {
                if (this->process != nullptr) {
                    //std::cout << "Entered." << std::endl;
                    if (this->process->getCurrentInstructionLine() != this->process->getTotalLinesOfCode()) {

                        //std::cout << "Entered2." << std::endl;
                        this->process->nextLine();  // Process next instruction
                    }
                    else {
                        MemoryManager::getInstance()->deallocate(this->process->getMemoryPointer(), this->process->getMemoryRequired(), this->process->getName());
                        this->process->setMemoryPointer(nullptr);
                        this->running = false;
                        break;
                    }
                }
            }
            this->cpuCycles++;
        }
    }
    // Round Robin
    else if (scheduler == "rr") {
        for (long long i = 0; i < this->quantum_cycles && CPUScheduler::getInstance()->getIsRunning(); i++) {
            if (this->cpuCycles == delay_per_exec) {
                if (this->process->getCurrentInstructionLine() != this->process->getTotalLinesOfCode()) {
                    this->process->nextLine();  // Process next instruction
                }
                else if (this->process->getCurrentInstructionLine() == this->process->getTotalLinesOfCode()) {
                    this->process->setState(Process::ProcessState::TERMINATED);
                    this->process->setCPUCoreID(NULL);
                    break;
                }
                else {
                    this->process->setState(Process::ProcessState::READY);
                    this->process->setCPUCoreID(NULL);
                    break;
                }
                this->cpuCycles = -1;
            }
            this->cpuCycles++;
        }

        if (this->process->getState() != Process::ProcessState::TERMINATED && this->process->getCurrentInstructionLine() == this->process->getTotalLinesOfCode()) {
            this->process->setState(Process::ProcessState::TERMINATED);
            this->process->setCPUCoreID(NULL);
        }
    }
    
    this->running = false;
}

//
//void CPUWorker::startWorker() {
//    {
//        std::lock_guard<std::mutex> lock(mtx);
//        this->running = true;  // Start the worker
//    }
//    if (this->process) {
//        this->process->setState(Process::ProcessState::RUNNING);
//    }
//    else {
//        this->running = false;
//    }
//
//    this->cpuCycles = 0;
//    long long rrCycles = 0;
//
//    while (this->running) {
//        // Make sure the process is valid before accessing it
//        if (!this->process) {
//            break;
//        }
//
//        // Check for stopping condition (e.g., quantum time or process completion)
//        if (scheduler == "rr" && rrCycles >= this->quantum_cycles) {
//            this->running = false;  // Stop after quantum time
//            break;
//        }
//
//        // Execute the current instruction
//        if (cpuCycles == delay_per_exec) {
//            if (this->process) {
//                if (this->process->getCurrentInstructionLine() < this->process->getTotalLinesOfCode()) {
//                    this->process->nextLine();  // Process next instruction
//                }
//                else {
//                    std::lock_guard<std::mutex> lock(mtx);
//                    this->running = false;  // Stop if process has finished executing
//                }
//            }
//            this->cpuCycles = -1;
//        }
//
//        // Increment/Reset cpuCycle counter
//        this->cpuCycles++;
//        rrCycles++;  // Increment round-robin cycle count
//    }
//    this->running = false;
//}


// Check if the worker has a process assigned
bool CPUWorker::hasProcess() {
    std::lock_guard<std::mutex> lock(mtx);
    return (this->process != nullptr);
}

// Return the current process (as shared_ptr)
std::shared_ptr<Process> CPUWorker::getProcess() {
    std::lock_guard<std::mutex> lock(mtx);
    return this->process;
}

int CPUWorker::getID() {
    return this->id;
}


bool CPUWorker::isRunning() const {
    return this->running;
}