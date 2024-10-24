#include "CPUWorker.h"
#include "ProcessManager.h"

CPUWorker* CPUWorker::sharedInstance = nullptr; // Initialize static member

CPUWorker::CPUWorker(int id, int delay_per_exec) {
    this->id = id;
    this->delay_per_exec = delay_per_exec;
    this->running = false;
}

// Set a CPU's process
void CPUWorker::setProcess(Process* process) {
    // Implementation
    this->running = false;
    this->process = process;
    this->process->setCPUCoreID(this->id);
}

// START THREADS in CPUscheduler WITH THIS function
void CPUWorker::startWorker() {
    this->running = true;
    this->process->setState(Process::RUNNING);

    while(this->running == true) {
        // Execution Here
        this->process->nextLine();

        if(this->process->getCurrentInstructionLine() == this->process->getTotalLinesOfCode()) {
            this->running = false;
            this->process->setState(Process::TERMINATED);
        }
        else {
                this->process->setState(Process::READY);
            }

        std::this_thread::sleep_for(std::chrono::milliseconds(this->delay_per_exec));
    }

    
    
}

bool CPUWorker::hasProcess() {
    if(this->process == nullptr) {
        return false;
    }
    else {
        return true;
    }
}

Process* CPUWorker::getProcess() {
    return this->process;
}
