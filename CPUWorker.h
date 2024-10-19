//#pragma once
//#include <iostream>
//#include <thread>
//#include <functional>
//#include <atomic>
//#include <vector>
//#include <queue>
//#include "Process.h"
//
//class CPUWorker
//{
//public:
//    CPUWorker(int id);            // Constructor
//    ~CPUWorker() = default; // Destructor
//    void setProcess(Process* process);
//    void startWorker();
//    bool hasProcess();
//
//private:
//    CPUWorker(const CPUWorker&); // Copy constructor
//    CPUWorker& operator=(const CPUWorker&); // Assignment operator
//    static CPUWorker* sharedInstance;
//    int id;
//
//    Process* process = nullptr;
//};
