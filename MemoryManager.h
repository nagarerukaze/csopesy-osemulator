#pragma once
#include <vector>
#include "Process.h"

typedef std::string String;

class MemoryManager
{
public:
	void initialize(size_t totalMemory, size_t frameSize, size_t memPerProc);
	static MemoryManager* getInstance();
	bool allocateMemory(Process* process);
	void freeMemory(Process* process);
	size_t calculateExternalFragmentation() const;
	void printMemory() const;
private:

	MemoryManager();
	MemoryManager(size_t totalMemory, size_t frameSize, size_t memPerProc);
	MemoryManager(const MemoryManager&);
	MemoryManager& operator = (const MemoryManager&);
	~MemoryManager() = default;
	static MemoryManager* sharedInstance;

	size_t totalMemory;
	size_t frameSize;
	size_t memPerProc;
	size_t numFrames;
	std::vector<String> memoryBlocks;
};

