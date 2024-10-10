#include "CPUWorker.h"

CPUWorker::CPUWorker () {}
CPUWorker::~CPUWorker() {}

void CPUWorker::runWorker() {
	while (this->running) {
		std::this_thread::sleep_for(std::chrono::milliseconds(17));
	}
}

int CPUWorker::getCoreID() const {
	return this->coreID;
}

