#include "ProcessManager.h"
#include "CPUScheduler.h"

ProcessManager::ProcessManager() {}

// Singleton stuff
ProcessManager::ProcessManager(long long batch_process_freq, long long min_ins, long long max_ins) {
    this->batch_process_freq = batch_process_freq;
    this->min_ins = min_ins;
    this->max_ins = max_ins;
    this->isGeneratingProcesses = false;
}
ProcessManager::ProcessManager(const ProcessManager&) {}

ProcessManager* ProcessManager::sharedInstance = nullptr;

void ProcessManager::initialize(long long batch_process_freq, long long min_ins, long long max_ins) {
    sharedInstance = new ProcessManager(batch_process_freq, min_ins, max_ins);
}

ProcessManager* ProcessManager::getInstance() {
    if (sharedInstance == NULL)
    {
        sharedInstance = new ProcessManager;
    }

    return sharedInstance;
}

void ProcessManager::createProcess(String name) {

    Process* new_process = new Process(name, rand() % (this->getMaxInstructions() - this->getMinInstructions() + 1) + this->getMinInstructions());
    activeProcesses.push_back(new_process);

    // Queue to CPU scheduler
    CPUScheduler::getInstance()->enqueueProcess(new_process);
}

Process* ProcessManager::findProcess(const String& name) const {
    auto it = std::find_if(activeProcesses.begin(), activeProcesses.end(),
        [&name](Process* process) { return process->getName() == name; });
    return (it != activeProcesses.end()) ? *it : nullptr;
}

bool ProcessManager::displayProcess(const String& name) const {
    Process* process = findProcess(name);

    // If Found: draw console
    if (process != nullptr) {
        process->draw();
        return true;
    }

    std::cout << "Process '" << name << "' not found." << std::endl;
    return false;
}

/*
    Used in `screen -ls` command.

    Display the activeProcesses (i.e., processes inside a CPU core).
*/
void ProcessManager::displayActiveProcessesList() {
    std::vector<CPUWorker*> workers = CPUScheduler::getInstance()->getCPUWorkers();

    for (const auto& worker : workers) {
        if (worker->hasProcess()) {
            Process* process = worker->getProcess();
            std::cout << process->getName() << "\t"
                << "(" << process->getTimestamp() << ") \t"
                << "Core: " << std::to_string(process->getCPUCoreID()) << "\t"
                << process->getCurrentInstructionLine() << "/" << process->getTotalLinesOfCode() << std::endl;
        }
    }
}

/*
    Used in `screen -ls` command.

    Display the processes inside the finishedProcesses list.
*/
void ProcessManager::displayFinishedProcessesList() {
    if (!this->finishedProcesses.empty()) {
        for (const auto& process : this->finishedProcesses) {
            std::cout << process->getName() << "\t"
                << "(" << process->getTimestamp() << ") \t"
                << "Finished!\t"
                << process->getCurrentInstructionLine() << "/" << process->getTotalLinesOfCode() << std::endl;
        }
    }
}

/*
    Used in `report-util` command.

    Print the activeProcesses (i.e., processes inside a CPU core).
*/
void ProcessManager::printActiveProcessesList(std::ofstream& outFile) {
    std::vector<CPUWorker*> workers = CPUScheduler::getInstance()->getCPUWorkers();

    for (const auto& worker : workers) {
        if (worker->hasProcess()) {
            Process* process = worker->getProcess();
            outFile << process->getName() << "\t"
                << "(" << process->getTimestamp() << ") \t"
                << "Core: " << std::to_string(process->getCPUCoreID()) << "\t"
                << process->getCurrentInstructionLine() << "/" << process->getTotalLinesOfCode() << std::endl;
        }
    }
}

/*
    Used in `report-util` command.

    Print the processes inside the finishedProcesses list.
*/
void ProcessManager::printFinishedProcessesList(std::ofstream& outFile) {
    if (!this->finishedProcesses.empty()) {
        for (const auto& process : this->finishedProcesses) {
            outFile << process->getName() << "\t"
                << "(" <<process->getTimestamp() << ") \t"
                << "Finished!\t"
                << process->getCurrentInstructionLine() << "/" << process->getTotalLinesOfCode() << std::endl;
        }
    }
}

/*
    Used in `screen -ls` command.

    Display the following information:
        (1) CPU Utilization %
        (2) # of cores used
        (3) # of cores available
        (4) List of active processes
        (5) List of finished processes

        For each process, display its:
            (1) Name
            (2) Time created
            (3) Core
            (4) Current line of instruction / total lines of code
*/
void ProcessManager::displayAllProcesses() {
    int coresUsed = CPUScheduler::getInstance()->getNumberOfCPUsUsed();
    int totalCores = CPUScheduler::getInstance()->getNumberOfCores();
    double cpuUtilization = (coresUsed / totalCores) * 100;

    std::cout << "CPU Utilization: " << (int)cpuUtilization << "%" << std::endl;

    std::cout << "Cores used: " << coresUsed << std::endl;
    std::cout << "Cores available: " << totalCores - coresUsed << std::endl;
    std::cout << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Running processes:" << std::endl;
    displayActiveProcessesList();

    std::cout << std::endl;

    std::cout << "Finished processes:" << std::endl;
    displayFinishedProcessesList();
    std::cout << "--------------------------------------" << std::endl;
}

void ProcessManager::moveToFinished(Process* processLala) {
    std::lock_guard<std::mutex> lock(mtx);

    int index = 0;

    for (Process* process : this->activeProcesses) {
    
        if (process == processLala) {

            // Move the process to finishedProcesses
            this->finishedProcesses.push_back(process);
     
            // Erase the process from activeProcesses using its index
            this->activeProcesses.erase(this->activeProcesses.begin() + index);
     
            return; // Exit after moving the process
        }
        else { 
            index++;
        }
            
    }
}

long long ProcessManager::getBatchProcessFreq() const {
    return this->batch_process_freq;
}

long long ProcessManager::getMinInstructions() const {
    return this->min_ins;
}

long long ProcessManager::getMaxInstructions() const {
    return this->max_ins;
}

bool ProcessManager::getIsGeneratingProcesses() const {
    return this->isGeneratingProcesses;
}

void ProcessManager::setIsGeneratingProcesses(bool val) {
    this->isGeneratingProcesses = val;
}