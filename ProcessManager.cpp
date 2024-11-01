#include "ProcessManager.h"
#include "CPUScheduler.h"

/*

    Singleton stuff

*/
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
    return sharedInstance;
}

void ProcessManager::createProcess(String name) {
    
    Process* new_process = new Process(name, rand() % (this->getMaxInstructions() - this->getMinInstructions() + 1) + this->getMinInstructions());
    activeProcesses.push_back(new_process);
    // queue to scheduler
    CPUScheduler::getInstance()->enqueueProcess(new_process);
}

bool ProcessManager::displayProcess(String name) const {
    // Find if process is in activeprocesses
    auto it = std::find_if(activeProcesses.begin(), activeProcesses.end(),
        [&name](Process* process) { return process->getName() == name; });

    // If Found: draw console
    if (it != activeProcesses.end()) {
        (*it)->draw();
        return true;
    }

    std::cout << "Process '" << name << "' not found." << std::endl;
    return false;
}

/*
    Used in `screen -ls` command.

    Display the processes inside a list.
    For each process, display its:
        (1) Name
        (2) TODO: time created or response time?
        (3) Core
        (4) Current line of instruction / total lines of code
*/
void ProcessManager::displayProcessesList(std::vector<Process*> processList) {
    for (const auto& process : processList) {
        std::cout << process->getName() << " \t"
                  << process->getTimestamp() << " \t"
                  //<< "Core: " << (process->getCPUCoreID() == -1 ? "N/A" : std::to_string(process->getCPUCoreID())) << " \t"
                  << process->getCurrentInstructionLine() << "/" << process->getTotalLinesOfCode() << std::endl;
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
*/
void ProcessManager::displayAllProcesses() {
    std::cout << std::endl << "CPU Utilization: " << std::endl;
    std::cout << "Cores used: " << CPUScheduler::getInstance()->getNumberOfCPUsUsed() << std::endl;
    std::cout << "Cores available: " << CPUScheduler::getInstance()->getNumberOfCores() << std::endl;
    std::cout << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Running processes:" << std::endl;
    displayProcessesList(this->activeProcesses);

    std::cout << std::endl;

    std::cout << "Finished processes:" << std::endl;
    displayProcessesList(this->finishedProcesses);
    std::cout << "--------------------------------------" << std::endl;
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
//#include "ProcessManager.h"
//
//ProcessManager::ProcessManager() {}
//ProcessManager::ProcessManager(const ProcessManager&) {}
//
//ProcessManager* ProcessManager::sharedInstance = nullptr;
//
//void ProcessManager::initialize() {
//    sharedInstance = new ProcessManager();
//}
//
//ProcessManager* ProcessManager::getInstance() {
//    return sharedInstance;
//}
//
//// screen -s <name of new process>
//void ProcessManager::createProcess(std::string name, int linesOfCode) {
//    // Create Process object
//    Process* process = new Process(name, linesOfCode);
//
//    activeProcesses.push_back(process);
//    //activeProcesses[name] = process;
//    CPUScheduler::getInstance()->enqueueProcess(process);
//}
//
//// screen -r <name of existing process>
//// Redraw the console of the associated process

void ProcessManager::moveToFinished(const std::string& processName) {
   auto it = std::find_if(activeProcesses.begin(), activeProcesses.end(),
       [&processName](Process* process) { return process->getName() == processName; });

   if (it != activeProcesses.end()) {
       finishedProcesses.push_back(*it); // Move to finishedProcesses
       activeProcesses.erase(it); // Erase from activeProcesses
   }
   else {
       std::cout << "Process '" << processName << "' not found in active processes." << std::endl;
   }
   //auto it = activeProcesses.find(processName); // Find the process

   //if (it != activeProcesses.end()) {
   //    finishedProcesses.push_back(it->second); // Move to finishedProcesses
   //    activeProcesses.erase(it); // Erase from activeProcesses
   //}
   //else {
   //    std::cout << "Process '" << processName << "' not found in active processes." << std::endl;
   //}
}
//
//void ProcessManager::displayAll() {
//    std::cout << "--------------------------------------" << std::endl;
//    std::cout << "Running processes:" << std::endl;
//
//    // Iterate over active processes and print details
//    for (const auto& process : activeProcesses) {
//        std::cout << process->getName() << " \t"
//            << process->getTimestamp() << " \t" // Add the timestamp
//            << "Core: " << (process->getCPUCoreID() == -1 ? "N/A" : std::to_string(process->getCPUCoreID())) << " \t"
//            << process->getCurrInstructionLine() << "/" << process->getLinesOfCode() << std::endl;
//    }
//
//    std::cout << std::endl;
//
//    // Iterate over finished processes and print details
//    for (const auto& process : finishedProcesses) {
//        std::cout << process->getName() << " \t"
//            << process->getTimestamp() << " \t" // Add the timestamp
//            << "Finished" << " \t"
//            << process->getCurrInstructionLine() << "/" << process->getLinesOfCode() << std::endl;
//    }
//
//    std::cout << "--------------------------------------" << std::endl;
//}
//
