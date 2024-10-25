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
   {    
        std::unique_lock<std::mutex> lock(mtx);

        // Signal to stop the current thread if it's running
        if (running) {
            running = false;
            cv.wait(lock, [this]() { return !running; }); // Wait until running is false
        }

        // Assign new process
        this->process = process;
        this->process->setCPUCoreID(this->id);
    }
}

// START THREADS in CPUscheduler WITH THIS function
void CPUWorker::startWorker() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        running = true;
    }
    this->process->setState(Process::ProcessState::RUNNING);

    while (true) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!running) {
                break; // Exit if running is false
            }
        }

        // Execution code
        this->process->nextLine();

        if (this->process->getCurrentInstructionLine() == this->process->getTotalLinesOfCode()) {
            this->process->setState(Process::TERMINATED);
            ProcessManager::getInstance()->moveToFinished(this->process->getName());
            this->process = nullptr;

            std::lock_guard<std::mutex> lock(mtx);
            running = false;
            cv.notify_one(); // Notify that worker has completed
            break;
        } else {
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
