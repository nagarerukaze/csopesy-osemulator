#include <string>
#include <vector>

#include "Process.h"

typedef std::string String;

class ProcessManager 
{
public:
    // Singleton stuff
    static void initialize(int batch_process_freq, int min_ins, int max_ins);
    static ProcessManager* getInstance();

    void createProcess(String name);
    int displayProcess(String name);
    void displayAllProcesses();
    Process* findProcess(const String& name);

    int getBatchProcessFreq() const;
    int getMinInstructions() const;
    int getMaxInstructions() const;
    bool getIsGeneratingProcesses() const;

    void setIsGeneratingProcesses(bool val);
private:
    ProcessManager(int batch_process_freq, int min_ins, int max_ins);
    ProcessManager(const ProcessManager&);
    ProcessManager& operator = (const ProcessManager&);
    ~ProcessManager() = default;
    static ProcessManager* sharedInstance;
    
    int batch_process_freq;
    int min_ins;
    int max_ins;
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
