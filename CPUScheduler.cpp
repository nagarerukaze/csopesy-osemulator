#include "CPUScheduler.h"
#include "ProcessManager.h"

CPUScheduler::CPUScheduler() {

}

CPUScheduler::CPUScheduler(const CPUScheduler&) {}

CPUScheduler* CPUScheduler::sharedInstance = nullptr;

void CPUScheduler::initialize(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec) {
	sharedInstance = new CPUScheduler();
    
    sharedInstance->scheduler = scheduler;
    sharedInstance->numberOfCores = num_cpu;
    sharedInstance->quantum_cycles = quantum_cycles;
    sharedInstance->delay_per_exec = delay_per_exec;
}

void CPUScheduler::initializeCPUWorkers(int num) {
    this->numberOfCores = num;
    for (int i = 0; i < num; i++) {
        CPUWorker* worker = new CPUWorker(i, this->delay_per_exec);
        this->cpuWorkers.push_back(worker);
    }
}

CPUScheduler* CPUScheduler::getInstance() {
	return sharedInstance;
}

void CPUScheduler::enqueueProcess(Process* process) {
    processQueue.push(process);
    std::cout << "Enqueued process: " << process->getName() << std::endl; // remove later
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

void CPUScheduler::FCFSScheduling() {
    while(running) {
        if(!processQueue.empty()) {
            for(int i = 0; i < this->numberOfCores; i++) {
                if(!cpuWorkers[i]->hasProcess() && !processQueue.empty()) {
                    Process* process = processQueue.front();
                    processQueue.pop();
                    
                    cpuWorkers[i]->setProcess(process);
                    std::thread thread([worker = cpuWorkers[i]]() { worker->startWorker(); });
                    thread.detach(); // Detach the thread
                }
            }
        }

        this->cpuCycles++;
        //std::this_thread::sleep_for(std::chrono::milliseconds(this->delay_per_exec));
    }
}

void CPUScheduler::RRScheduling() {
    while (running) {
        if (this->cpuCycles == this->quantum_cycles) {
            cpuCycles = 1;  // Reset the cycle counter
        }

        if (this->cpuCycles == 1) {  // Only switch processes at the start of each quantum cycle
            for (int i = 0; i < this->numberOfCores; i++) {
                if (!processQueue.empty()) {
                    Process* process_in = processQueue.front();
                    processQueue.pop();

                    // If a process is already assigned and isn't finished, put it back in the queue
                    if (cpuWorkers[i]->hasProcess()) {
                        Process* process_out = cpuWorkers[i]->getProcess();
                        if (process_out->getState() == Process::ProcessState::READY) {
                            processQueue.push(process_out);  // Re-queue the paused process
                        }
                    }

                    // Assign the new process and start it
                    cpuWorkers[i]->setProcess(process_in);
                    std::thread thread([worker = cpuWorkers[i]]() { worker->startWorker(); });
                    thread.detach();
                }
            }
        }

        this->cpuCycles++;
        //std::this_thread::sleep_for(std::chrono::milliseconds(this->quantum_cycles));
    }
}
