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

typedef std::string String;

class CPUWorker
{
public:
    CPUWorker(int id, long long delay_per_exec, String scheduler, long long quantum_cycles, String allocator);    // Constructor
    ~CPUWorker() = default;                         // Destructor
    void setProcess(std::shared_ptr<Process> process);  // Use shared_ptr
    void startWorker();
    bool hasProcess();
    void removeProcess();
    std::shared_ptr<Process> getProcess();  // Return shared_ptr
    bool isRunning() const;
    long long getActiveCPUTicks();
    int getID();
private:
    CPUWorker(const CPUWorker&);                    // Copy constructor
    CPUWorker& operator=(const CPUWorker&);         // Assignment operator
    int id;

    String scheduler;
    String allocator;
    long long delay_per_exec;
    long long quantum_cycles;
    std::shared_ptr<Process> process;  // Changed to shared_ptr
    std::mutex mtx;
    std::condition_variable cv;
    long long activeCPUTicks = 0;
    long long cpuCycles = 1;
    bool running = false;
};

