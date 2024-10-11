#include "CPUWorker.h"

CPUWorker* CPUWorker::sharedInstance = nullptr; // Initialize static member

CPUWorker::CPUWorker(int id) {
    this->id = id;
}

// Set a CPU's process
void CPUWorker::setProcess(Process* process) {
    // Implementation
    this->process = process;
    this->process->setCPUCoreID(this->id);
}

// START THREADS in CPUscheduler WITH THIS function
void CPUWorker::startWorker() {
    // 
    int currentLine = this->process->getCurrInstructionLine();
    int totalLines = this->process->getLinesOfCode();

    while(currentLine < totalLines) {
        this->process->printToTextFile();
        currentLine = this->process->getCurrInstructionLine();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "Finished process.";

    this->process = nullptr;
}

bool CPUWorker::hasProcess() {
    if(this->process == nullptr) {
        return false;
    }
    else {
        return true;
    }
}
