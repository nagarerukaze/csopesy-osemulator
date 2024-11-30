#pragma once
#include <iostream>
#include <fstream>
#include <thread>
#include <functional>
#include <atomic>
#include <vector>
#include <queue>
#include <mutex>
#include "CPUWorker.h"
#include "MemoryManager.h"
#include "PagingAllocator.h"

typedef std::string String;

class CPUScheduler
{
public:
    static std::atomic<long long> cpuCycles;
    void initialize(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec, String allocator);
    void initializeCPUWorkers(int numberOfCPUs);
    static CPUScheduler* getInstance();
	void startScheduler();
    void stopScheduler();
    void enqueueProcess(std::shared_ptr<Process> process);
    int getNumberOfCPUsUsed();
    int getNumberOfCores();
    bool getIsRunning();
    long long getQuantumCycles();
    std::vector<CPUWorker*> getCPUWorkers();
    size_t getIdleCPUTicks() const;
    size_t getActiveCPUTicks() const;

private:
    CPUScheduler();
    CPUScheduler(String scheduler, int num_cpu, long long quantum_cycles, long long delay_per_exec, String allocator);
    CPUScheduler(const CPUScheduler&);
    CPUScheduler& operator = (const CPUScheduler&);
    ~CPUScheduler() = default;
    static CPUScheduler* sharedInstance;

    std::atomic<bool> running{ true };
    std::queue<std::shared_ptr<Process>> processQueue;
    std::mutex mtx;

    String scheduler;
    String allocator;
    int numberOfCores;
    long long quantum_cycles;
    long long delay_per_exec;
    size_t idleCPUTicks;
    size_t activeCPUTicks;

    std::vector<CPUWorker*> cpuWorkers;
    std::vector<std::thread> workerThreads;
    
    void FCFSScheduling();
    void RRScheduling();
    void handleProcessOut(CPUWorker* worker, std::shared_ptr<Process>& process_out);
};

