#pragma once
#include <iostream>
#include <thread>
#include <functional>
#include <atomic>
#include <vector>
#include <queue>
#include <mutex>
#include <memory>  // For std::shared_ptr
#include "Process.h"

class CPUWorker
{
public:
    CPUWorker(int id, long long delay_per_exec);    // Constructor
    ~CPUWorker() = default;                         // Destructor
    void setProcess(std::shared_ptr<Process> process);  // Use shared_ptr
    void startWorker();
    bool hasProcess();
    std::shared_ptr<Process> getProcess();  // Return shared_ptr

private:
    CPUWorker(const CPUWorker&);                    // Copy constructor
    CPUWorker& operator=(const CPUWorker&);         // Assignment operator
    int id;
    std::atomic<bool> running;

    long long delay_per_exec;
    std::shared_ptr<Process> process;  // Changed to shared_ptr
    std::mutex mtx;
    std::condition_variable cv;
    long long cpuCycles = 1;
};
