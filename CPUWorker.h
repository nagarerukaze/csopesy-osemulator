#pragma once
#include <thread>

class CPUWorker
{
public:

	CPUWorker();
	~CPUWorker();

	void runWorker();

	int getCoreID() const;
	
private:

	bool running = true;
	int coreID;
};

