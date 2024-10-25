#include "CPUScheduler.h"
#include "ProcessManager.h"

CPUScheduler::CPUScheduler() {

}

CPUScheduler::CPUScheduler(const CPUScheduler&) {}

CPUScheduler* CPUScheduler::sharedInstance = nullptr;

void CPUScheduler::initialize(String scheduler, int num_cpu, int quantum_cycles, int delay_per_exec) {
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
        std::this_thread::sleep_for(std::chrono::milliseconds(this->delay_per_exec));
    }
}

void CPUScheduler::RRScheduling() {
    while(running) {
        std::cout << "[CYCLE: " << this->cpuCycles << "]" << std::endl; 
        for(int i = 0; i < this->numberOfCores; i++) {
            if(!processQueue.empty()) {
                Process* process_in = processQueue.front();
                processQueue.pop();

                if(cpuWorkers[i]->hasProcess()) {
                    Process* process_out = cpuWorkers[i]->getProcess();

                    if(process_out->getState() != Process::TERMINATED) {
                        processQueue.push(process_out);
                        std::cout << "Processor #" << i + 1 << "/" << this->numberOfCores << "\tProcess: " << process_in->getName() << "\tLine: " << process_in->getCurrentInstructionLine()<< "/" << process_in->getTotalLinesOfCode() << "\t(Preempteed)" << std::endl;
                    }
                }
                else {
                    std::cout << "Processor #" << i + 1 << "/" << this->numberOfCores << "\tProcess: " << process_in->getName() << "\tLine: " << process_in->getCurrentInstructionLine()<< "/" << process_in->getTotalLinesOfCode() << "\t(Free)" << std::endl;
                }
                
                cpuWorkers[i]->setProcess(process_in);
                std::thread thread([worker = cpuWorkers[i]]() { worker->startWorker(); });
                thread.detach(); // Detach the thread
            }
        }

        std::cout << "----------------------------------------------------------------------------------------" << std::endl;
        this->cpuCycles++;
        std::this_thread::sleep_for(std::chrono::milliseconds(this->quantum_cycles));   
    }
}
