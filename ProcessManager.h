#pragma once
#include <string>
#include <unordered_map>
#include "Process.h"

class ProcessManager
{
public:
    void createProcess(std::string name, int linesOfCode);
    void displayProcess(Process process) const;

private:
    ProcessManager();
    ~ProcessManager() = default;

    std::unordered_map<std::string, Process> activeProcesses;
    //std::unordered_map<std::string, Process> finishedProcesses;
};
