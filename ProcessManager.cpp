#include "ProcessManager.h"

#include <iostream>

#include "Process.h"

ProcessManager::ProcessManager() {}

// screen -s <name of new process>
void ProcessManager::createProcess(std::string name, int linesOfCode) {
    // Create Process object
    //Add to activeProcesses
}

// screen -r <name of existing process>
// Redraw the console of the associated process
void ProcessManager::displayProcess(Process process) const {
}
