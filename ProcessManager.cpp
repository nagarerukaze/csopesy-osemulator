#include "ProcessManager.h"
#include "Process.h"
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
    //...idk if we're supposed to:
    //Process* process = new Process(name, currInstructionLine, linesOfCode);
    //Add to activeProcesses
    activeProcesses[name] = process;
}

// screen -r <name of existing process>
// Redraw the console of the associated process


void ProcessManager::displayProcess(std::string processName) const {
    auto process = activeProcesses.find(processName);

    if (process != activeProcesses.end() && process->second != nullptr) {
        // change to calling display process from process instance through dereferencing
        process->second->draw();
    }
        
    else {
        std::cout << "Process '" << processName << "' not found." << std::endl;
    }
}
