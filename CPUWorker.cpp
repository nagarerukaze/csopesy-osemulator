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

// Set a CPU's process (now accepts shared_ptr<Process>)
void CPUWorker::setProcess(std::shared_ptr<Process> process) {
    std::unique_lock<std::mutex> lock(mtx);

    // Signal to stop the current thread if it's running
    if (this->running) {
        this->running = false;
    }

    // Assign new process
    this->process = process;
    if (this->process) {
        this->process->setCPUCoreID(this->id);
    }
}

// RR lang muna
void CPUWorker::startWorker() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        this->cpuCycles = 0;
        this->running = true;
        if (this->process) {
            this->process->setState(Process::ProcessState::RUNNING);
            this->process->setCPUCoreID(this->id);
        }
    }
    for (long long i = 0; i < this->quantum_cycles && CPUScheduler::getInstance()->getIsRunning(); i++) {
        if (this->cpuCycles == delay_per_exec) {
            if (this->process->getCurrentInstructionLine() < this->process->getTotalLinesOfCode()) {
                this->process->nextLine();  // Process next instruction
            }
            else if (this->process->getCurrentInstructionLine() == this->process->getTotalLinesOfCode()) {
                break;
            }
            this->cpuCycles = -1;
        }

        this->cpuCycles++;
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


bool CPUWorker::isRunning() const {
    return this->running;
}