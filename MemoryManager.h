#pragma once
#include <vector>
#include "Process.h"

typedef std::string String;

class MemoryManager
{
public:
	MemoryManager(size_t totalMemory, size_t frameSize, size_t memPerProc);
	bool allocateMemory(Process* process);
	void freeMemory(Process* process);
	size_t calculateExternalFragmentation() const;
	void printMemory() const;
	size_t totalMemory;
	size_t frameSize;
	size_t memPerProc;
	size_t numFrames;
	std::vector<String> memoryBlocks;
private:
	
};

