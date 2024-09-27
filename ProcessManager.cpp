#include "ProcessManager.h"
#include "Process.h"
#include "Process.cpp"
#include <iostream>

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
