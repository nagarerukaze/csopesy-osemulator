#include "CPUScheduler.h"

CPUScheduler::CPUScheduler() {}
CPUScheduler::CPUScheduler(const CPUScheduler&) {}

CPUScheduler* CPUScheduler::sharedInstance = nullptr;

void CPUScheduler::initialize() {
	sharedInstance = new CPUScheduler();
}

CPUScheduler* CPUScheduler::getInstance() {
	return sharedInstance;
}

void CPUScheduler::startWorkerThreads() {

	CPUWorker cpuCore1 = CPUWorker();
	CPUWorker cpuCore2 = CPUWorker();
	CPUWorker cpuCore3 = CPUWorker();
	CPUWorker cpuCore4 = CPUWorker();

	std::thread coreThread1(&CPUWorker::runWorker, cpuCore1);
	std::thread coreThread2(&CPUWorker::runWorker, cpuCore2);
	std::thread coreThread3(&CPUWorker::runWorker, cpuCore3);
	std::thread coreThread4(&CPUWorker::runWorker, cpuCore4);

	coreThread1.detach();
	coreThread2.detach();
	coreThread3.detach();
	coreThread4.detach();
}

int CPUScheduler::assignCPUWorker() {
	return 0;
}

