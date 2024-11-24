#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>

#include "Process.h"

typedef std::string String;

class MemoryManager
{
public:
	void initialize(size_t maximumSize);
	static MemoryManager* getInstance();


	void* allocate(size_t size, String process);
	void deallocate(void* ptr, size_t size, String process);
	size_t getMaximumMemory();
	String visualizeMemory();
	// void printMemory(long long qq);
	// void printASCIIMemory(std::ofstream& outFile);
	String getCurrentTime();
private:

	MemoryManager();
	MemoryManager(size_t totalMemory);
	MemoryManager(const MemoryManager&);
	MemoryManager& operator = (const MemoryManager&);
	~MemoryManager();
	static MemoryManager* sharedInstance;

	size_t maximumSize;
	size_t allocatedSize;
	std::vector<char> memory;
	std::vector<String> strProcessesInMemory;
	std::unordered_map<size_t, bool> allocationMap;

	void initializeMemory();
	bool canAllocateAt(size_t index, size_t size) const;
	void allocateAt(size_t index, size_t size);
	void deallocateAt(size_t index, size_t size);
};

