#include <string>
#include <vector>

#include "Process.h"

typedef std::string String;

class ProcessManager 
{
public:
    // Singleton stuff
    static void initialize(long long batch_process_freq, long long min_ins, long long max_ins);
    static ProcessManager* getInstance();

    void createProcess(String name);
    bool displayProcess(String name) const;
    void displayProcessesList(std::vector<Process*> processList);
    void displayAllProcesses();
    Process* findProcess(const String& name);

    long long getBatchProcessFreq() const;
    long long getMinInstructions() const;
    long long getMaxInstructions() const;
    bool getIsGeneratingProcesses() const;
    void moveToFinished(const std::string& processName);

    void setIsGeneratingProcesses(bool val);

private:
    ProcessManager(long long batch_process_freq, long long min_ins, long long max_ins);
    ProcessManager(const ProcessManager&);
    ProcessManager& operator = (const ProcessManager&);
    ~ProcessManager() = default;
    static ProcessManager* sharedInstance;
    
    long long batch_process_freq;
    long long min_ins;
    long long max_ins;
    bool isGeneratingProcesses;

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
