#pragma once
#include <iostream>
#include <fstream>
#include <thread>
#include <functional>
#include <atomic>
#include <vector>
#include <queue>
#include "CPUWorker.h"


class CPUScheduler
{
public:

    void initialize(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec);
    void initializeCPUWorkers(int numberOfCPUs);
    static CPUScheduler* getInstance();
	void startScheduler();
    void enqueueProcess(Process* process);
    int getNumberOfCPUsUsed();
    int getNumberOfCores();

private:
    CPUScheduler(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec);
    CPUScheduler(const CPUScheduler&);
    CPUScheduler& operator = (const CPUScheduler&);
    ~CPUScheduler() = default;
    static CPUScheduler* sharedInstance;

    long long cpuCycles = 0;
    String scheduler;
    int numberOfCores;
    long long quantum_cycles;
    long long delay_per_exec;

    void workerFunction(int coreID);
    std::vector<std::thread> workerThreads;
    std::vector<CPUWorker*> cpuWorkers;
    std::atomic<bool> running{ true };
    std::queue<Process*> processQueue;

    void FCFSScheduling();
    void RRScheduling();

};
