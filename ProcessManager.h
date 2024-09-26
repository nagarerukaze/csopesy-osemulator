#pragma once
#include <string>
#include <unordered_map>
#include "Process.h"

class ProcessManager
{
public:
    //typedef std::string String;
    //typedef std::unordered_map<std::string, Process> ProcessTable;

    static ProcessManager* getInstance();
    void createProcess(std::string name, int linesOfCode);
    void displayProcess(Process process) const;

    std::unordered_map<std::string, Process> getActiveProcesses;

private:
    ProcessManager();
    ProcessManager(const ProcessManager&);
    ProcessManager& operator = (const ProcessManager&);
    ~ProcessManager() = default;
    static ProcessManager* sharedInstance;

    std::unordered_map<std::string, Process> activeProcesses;
    //std::unordered_map<std::string, Process> finishedProcesses;
};
