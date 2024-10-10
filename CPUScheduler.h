#pragma once
#include <thread>
#include <functional>

#include "CPUWorker.h"

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
};

