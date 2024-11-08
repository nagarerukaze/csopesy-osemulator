#include "CPUWorker.h"
#include "ProcessManager.h"

CPUWorker::CPUWorker(int id, long long delay_per_exec) {
    this->id = id;
    this->delay_per_exec = delay_per_exec;
    this->running = false;
}

// Set a CPU's process (now accepts shared_ptr<Process>)
void CPUWorker::setProcess(std::shared_ptr<Process> process) {
    std::unique_lock<std::mutex> lock(mtx);

    // Signal to stop the current thread if it's running
    if (running) {
        running = false;
    }

    // Assign new process
    this->process = process;
    if (this->process) {
        this->process->setCPUCoreID(this->id);
    }
}

// Start the worker's thread and process execution
void CPUWorker::startWorker() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        running = true;
    }
    if (this->process) {
        this->process->setState(Process::ProcessState::RUNNING);
    }
    this->cpuCycles = 0;

    while (true) {
        // Check if the worker should stop running
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!running || !this->process) {
                break; // Exit if running is false or no process is assigned
            }
        }

        // Execute the current instruction
        if (cpuCycles == delay_per_exec && this->process) {
            if (this->process->getCurrentInstructionLine() < this->process->getTotalLinesOfCode()) {
                this->process->nextLine();
            }
            else {
                running = false;
            }
            this->cpuCycles = -1;
        }

        // Increment/Reset cpuCycle counter
        this->cpuCycles++;
    }
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
