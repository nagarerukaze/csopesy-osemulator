#pragma once
#include <thread>
#include <functional>
#include <atomic>
#include "Process.h"

class CPUScheduler
{
public:

    void initialize();
    static CPUScheduler* getInstance();
	void startWorkerThreads();
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
};

