#pragma once
#include <thread>
#include <functional>
#include <atomic>
#include <vector>
#include <queue>
#include "Process.h"

class CPUScheduler
{
public:

    void initialize();
    static CPUScheduler* getInstance();
	void startWorkerThreads();
    void enqueueProcess(Process* process);
    int assignCPUWorker();

private:
    CPUScheduler();
    CPUScheduler(const CPUScheduler&);
    CPUScheduler& operator = (const CPUScheduler&);
    ~CPUScheduler() = default;
    static CPUScheduler* sharedInstance;

    void workerFunction(int coreID);
    std::vector<std::thread> workerThreads;
    std::atomic<bool> running{ true };
    std::queue<Process*> processQueue;
};

