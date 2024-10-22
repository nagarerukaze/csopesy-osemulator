#include <string>
#include <vector>

#include "Process.h"

class ProcessManager 
{
public:
    void createProcess(std::string name, int totalLines);
    int displayProcess(std::string name);
    void displayAllProcesses();
    Process* findProcess(const std::string& name);
private:
    std::vector<Process*> activeProcesses;
    std::vector<Process*> finishedProcesses;
};

//#pragma once
//#include <iostream>
//#include <string>
//#include <unordered_map>
//#include <vector>
//#include <algorithm>
//#include "Process.h"
//#include "CPUScheduler.h"
//
//class ProcessManager
//{
//public:
//    //typedef std::string String;
//    //typedef std::unordered_map<std::string, Process> ProcessTable;
//    static void initialize();
//    static ProcessManager* getInstance();
//    static void draw();
//    void createProcess(std::string name, int linesOfCode);
//    int displayProcess(std::string processName) const;
//    void moveToFinished(const std::string& processName) ;
//    void displayAll();
//
//    //std::unordered_map<std::string, Process> getActiveProcesses;
//
//private:
//    ProcessManager();
//    ProcessManager(const ProcessManager&);
//    ProcessManager& operator = (const ProcessManager&);
//    ~ProcessManager() = default;
//    static ProcessManager* sharedInstance;
//
//    // change Process to Process*
//    // std::vector<std::string, Process*> activeProcesses;
//    std::vector<Process*> activeProcesses;
//    std::vector<Process*> finishedProcesses;
//};
