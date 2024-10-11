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

    activeProcesses[name] = process;
    CPUScheduler::getInstance()->enqueueProcess(process);
}

// screen -r <name of existing process>
// Redraw the console of the associated process

int ProcessManager::displayProcess(std::string processName) const {
    auto process = activeProcesses.find(processName);

    if (process != activeProcesses.end() && process->second != nullptr) {
        // change to calling display process from process instance through dereferencing
        process->second->draw();
        return 1;
    }
        
    else {
        std::cout << "Process '" << processName << "' not found." << std::endl;
        return 0;
    }
}

void ProcessManager::moveToFinished(const std::string& processName) {
    auto it = activeProcesses.find(processName); // Find the process

    if (it != activeProcesses.end()) {
        finishedProcesses.push_back(it->second); // Move to finishedProcesses
        activeProcesses.erase(it); // Erase from activeProcesses
    }
    else {
        std::cout << "Process '" << processName << "' not found in active processes." << std::endl;
    }
}

void ProcessManager::displayAll() {
    std::cout << "Active Processes:" << std::endl;
    for (auto i = this->activeProcesses.begin(); i != this->activeProcesses.end(); i++)
        std::cout << i->first << " \t" << i->second->getCPUCoreID() << " \t" << i->second->getCurrInstructionLine() << "/" << i->second->getLinesOfCode() << std::endl;

    std::cout << "Finished Processes:" << std::endl;
    for (auto it = finishedProcesses.begin(); it != finishedProcesses.end(); ++it) {
        std::cout << (*it)->getName() << " \t" << (*it)->getCPUCoreID() << " \t"
            << (*it)->getCurrInstructionLine() << "/" << (*it)->getLinesOfCode() << std::endl;
    }
}
