#pragma once
#include <iostream>
#include <thread>
#include <functional>
#include <atomic>
#include <vector>
#include <queue>
#include "Process.h"
#include "CPUWorker.h"

class CPUScheduler
{
public:

    void initialize();
    void initializeCPUWorkers(int numberOfCPUs);
    static CPUScheduler* getInstance();
	void startScheduler();
    void enqueueProcess(Process* process);
    int assignCPUWorker();

private:
    CPUScheduler();
    CPUScheduler(const CPUScheduler&);
    CPUScheduler& operator = (const CPUScheduler&);
    ~CPUScheduler() = default;
    static CPUScheduler* sharedInstance;

    int numberOfCores;

    void workerFunction(int coreID);
    std::vector<std::thread> workerThreads;
    std::vector<CPUWorker*> cpuWorkers;
    std::atomic<bool> running{ true };
    std::queue<Process*> processQueue;
};

