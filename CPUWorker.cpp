#include "CPUWorker.h"
#include "ProcessManager.h"

CPUWorker* CPUWorker::sharedInstance = nullptr; // Initialize static member

CPUWorker::CPUWorker(int id, long long delay_per_exec) {
    this->id = id;
    this->delay_per_exec = delay_per_exec;
    this->running = false;

}

// Set a CPU's process
void CPUWorker::setProcess(Process* process) {
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

// START THREADS in CPUscheduler WITH THIS function
void CPUWorker::startWorker() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        running = true;
    }
    if (this->process) {
        this->process->setState(Process::ProcessState::RUNNING);
    }
    this->cpuCycles = 1;

    while (true) {
        // Check if the worker should stop running
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!running || !this->process) { 
                break; // Exit if running is false or no process is assigned
            }
        }

        // Execute the current quantum
        if (cpuCycles == 1 && this->process) {
            if (this->process->getCurrentInstructionLine() < this->process->getTotalLinesOfCode()) {
                this->process->nextLine();
            }
            else {
                running = false;
            }
            
        }

        // Increment the cycle counter and reset if needed
        this->cpuCycles++;
        if (this->cpuCycles > delay_per_exec) {
            this->cpuCycles = 1;
        }

        // Quantum delay
        //std::this_thread::sleep_for(std::chrono::milliseconds(this->delay_per_exec));
    }
}

bool CPUWorker::hasProcess() {
    std::lock_guard<std::mutex> lock(mtx);
    return (this->process != nullptr);
}

Process* CPUWorker::getProcess() {
    std::lock_guard<std::mutex> lock(mtx);
    return this->process;
}
