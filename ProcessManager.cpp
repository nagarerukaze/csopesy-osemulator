#include "ProcessManager.h"

#include <iostream>

#include "Process.h"

ProcessManager::ProcessManager() {}

// screen -s <name of new process>
void ProcessManager::createProcess(std::string name, int linesOfCode) {
    linesOfCode = 100;

    // Create Process object
    Process process = Process(name, linesOfCode);
    //...idk if we're supposed to:
    //Process* process = new Process(name, currInstructionLine, linesOfCode);
    //Add to activeProcesses
    activeProcesses.insert({ name, process });
}

// screen -r <name of existing process>
// Redraw the console of the associated process
void ProcessManager::displayProcess(Process process) const {
    if (activeProcesses.find(process.getName()) == activeProcesses.end()) {
        std::cout << "Process name: " << process.getName() << std::endl;
        std::cout << "Current line of instruction / Total line of instruction: " << process.getCurrInstructionLine() << "/ " << process.getLinesOfCode() << std::endl;
        std::cout << "Timestamp: " << process.getTimestamp() << std::endl;
    }
    else {
        std::cout << "Process '" << process.getName() << "' not found." << std::endl;
    }
}
