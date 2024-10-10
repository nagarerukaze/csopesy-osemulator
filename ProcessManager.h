#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include "Process.h"

class ProcessManager
{
public:
    //typedef std::string String;
    //typedef std::unordered_map<std::string, Process> ProcessTable;
    static void initialize();
    static ProcessManager* getInstance();
    static void draw();
    void createProcess(std::string name, int linesOfCode);
    int displayProcess(std::string processName) const;

    //std::unordered_map<std::string, Process> getActiveProcesses;

private:
    ProcessManager();
    ProcessManager(const ProcessManager&);
    ProcessManager& operator = (const ProcessManager&);
    ~ProcessManager() = default;
    static ProcessManager* sharedInstance;

    // change Process to Process*
    std::unordered_map<std::string, Process*> activeProcesses;
    //std::unordered_map<std::string, Process> finishedProcesses;
};
