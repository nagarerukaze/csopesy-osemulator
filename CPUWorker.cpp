#include "CPUWorker.h"
#include "ProcessManager.h"
#include "CPUScheduler.h"
#include "MemoryManager.h"


CPUWorker::CPUWorker(int id, long long delay_per_exec, String scheduler, long long quantum_cycles, String allocator) {
    this->id = id;
    this->delay_per_exec = delay_per_exec;
    this->running = false;
    this->scheduler = scheduler;
    this->quantum_cycles = quantum_cycles;
    this->process = nullptr;
    this->allocator = allocator;
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
    if (this->process != nullptr) {
        //std::cout << "Inside worker: Process is not null" << std::endl;
        std::lock_guard<std::mutex> lock(mtx);
        this->process->setState(Process::ProcessState::RUNNING);
        this->process->setCPUCoreID(this->id);
    }
    std::lock_guard<std::mutex> lock(mtx);
    this->cpuCycles = 0;
    // FCFS
    if (scheduler == "fcfs") {
        while (this->process->getCurrentInstructionLine() != this->process->getTotalLinesOfCode() && CPUScheduler::getInstance()->getIsRunning()) {
            if (this->cpuCycles == delay_per_exec && this->process != nullptr) {
                if(this->process->getCurrentInstructionLine() != this->process->getTotalLinesOfCode()) {
                    this->process->nextLine();  // Process next instruction
                }

                if (this->process->getCurrentInstructionLine() == this->process->getTotalLinesOfCode()) {
                    break;
                }
                this->cpuCycles = -1;
            }
            this->cpuCycles++;
        }
    }
    // Round Robin
    else if (scheduler == "rr") {
        for (long long i = 0; i < this->quantum_cycles && CPUScheduler::getInstance()->getIsRunning(); i++) {
            if (this->cpuCycles == delay_per_exec && this->process != nullptr) {
                if (this->process->getCurrentInstructionLine() != this->process->getTotalLinesOfCode()) {
                    this->process->nextLine();  // Process next instruction
                }
                
                if (this->process->getCurrentInstructionLine() == this->process->getTotalLinesOfCode()) {
                    break;
                }
                this->cpuCycles = -1;
            }
            this->cpuCycles++;
        }
    }

    if (this->process->getCurrentInstructionLine() == this->process->getTotalLinesOfCode()) {
        this->process->setCPUCoreID(NULL);
        this->process->setState(Process::ProcessState::TERMINATED);
        
        // Flat Memory Allocator
        if (allocator == "flat") {
            MemoryManager::getInstance()->deallocate(this->process->getMemoryPointer(), this->process->getMemoryRequired(), this->process->getName());
            this->process->setMemoryPointer(nullptr);
        }

        // Paging Allocator
    }
    
    this->running = false;
}

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