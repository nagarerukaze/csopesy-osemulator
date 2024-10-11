#include "ProcessManager.h"

ProcessManager::ProcessManager() {}
ProcessManager::ProcessManager(const ProcessManager&) {}

ProcessManager* ProcessManager::sharedInstance = nullptr;

void ProcessManager::initialize() {
    sharedInstance = new ProcessManager();
}

ProcessManager* ProcessManager::getInstance() {
    return sharedInstance;
}

// screen -s <name of new process>
void ProcessManager::createProcess(std::string name, int linesOfCode) {
    // Create Process object
    Process* process = new Process(name, linesOfCode);

    activeProcesses.push_back(process);
    //activeProcesses[name] = process;
    CPUScheduler::getInstance()->enqueueProcess(process);
}

// screen -r <name of existing process>
// Redraw the console of the associated process

int ProcessManager::displayProcess(std::string processName) const {
    auto it = std::find_if(activeProcesses.begin(), activeProcesses.end(),
        [&processName](Process* process) { return process->getName() == processName; });

    if (it != activeProcesses.end()) {
        (*it)->draw(); // Call draw on the found process
        return 1;
    }

    std::cout << "Process '" << processName << "' not found." << std::endl;
    return 0;

    //auto process = activeProcesses.find(processName);

    //if (process != activeProcesses.end() && process->second != nullptr) {
    //    // change to calling display process from process instance through dereferencing
    //    process->second->draw();
    //    return 1;
    //}
    //    
    //else {
    //    std::cout << "Process '" << processName << "' not found." << std::endl;
    //    return 0;
    //}
}

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

void ProcessManager::displayAll() {
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Running processes:" << std::endl;

    // Iterate over active processes and print details
    for (const auto& process : activeProcesses) {
        std::cout << process->getName() << " \t"
            << process->getTimestamp() << " \t" // Add the timestamp
            << "Core: " << (process->getCPUCoreID() == -1 ? "N/A" : std::to_string(process->getCPUCoreID())) << " \t"
            << process->getCurrInstructionLine() << "/" << process->getLinesOfCode() << std::endl;
    }

    std::cout << std::endl;

    std::cout << "Finished processes:" << std::endl;

    // Iterate over finished processes and print details
    for (const auto& process : finishedProcesses) {
        std::cout << process->getName() << " \t"
            << process->getTimestamp() << " \t" // Add the timestamp
            << "Finished" << " \t"
            << process->getCurrInstructionLine() << "/" << process->getLinesOfCode() << std::endl;
    }

    std::cout << "--------------------------------------" << std::endl;
}

