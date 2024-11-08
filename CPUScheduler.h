#pragma once
#include <iostream>
#include <fstream>
#include <thread>
#include <functional>
#include <atomic>
#include <vector>
#include <queue>
#include "CPUWorker.h"

typedef std::string String;

class CPUScheduler
{
public:
    static std::atomic<long long> cpuCycles;
    void initialize(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec);
    void initializeCPUWorkers(int numberOfCPUs);
    static CPUScheduler* getInstance();
	void startScheduler();
    void stopScheduler();
    void enqueueProcess(std::shared_ptr<Process> process);
    int getNumberOfCPUsUsed();
    int getNumberOfCores();
    long long getQuantumCycles();
    std::vector<CPUWorker*> getCPUWorkers();

private:
    CPUScheduler();
    CPUScheduler(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec);
    CPUScheduler(const CPUScheduler&);
    CPUScheduler& operator = (const CPUScheduler&);
    ~CPUScheduler() = default;
    static CPUScheduler* sharedInstance;

    std::atomic<bool> running{ true };
    std::queue<std::shared_ptr<Process>> processQueue;
    std::mutex mtx;

    String scheduler;
    int numberOfCores;
    long long quantum_cycles;
    long long delay_per_exec;
    // long long cpuCycles;

    std::vector<CPUWorker*> cpuWorkers;
    std::vector<std::thread> workerThreads;
    
    void FCFSScheduling();
    void RRScheduling();
};
