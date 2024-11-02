#pragma once
#include <iostream>
#include <thread>
#include <functional>
#include <atomic>
#include <vector>
#include <queue>
#include <mutex>
#include "Process.h"

class CPUWorker
{
public:
    CPUWorker(int id, long long delay_per_exec);    // Constructor
    ~CPUWorker() = default;                         // Destructor
    void setProcess(Process* process);
    void startWorker();
    void switchProcess(Process* process);
    bool hasProcess();
    Process* getProcess();

private:
    CPUWorker(const CPUWorker&);                    // Copy constructor
    CPUWorker& operator=(const CPUWorker&);         // Assignment operator
    int id;
    std::atomic<bool> running;

    long long delay_per_exec;
    Process* process = nullptr;
    std::mutex mtx;
    std::condition_variable cv;
    long long cpuCycles = 1;
};
