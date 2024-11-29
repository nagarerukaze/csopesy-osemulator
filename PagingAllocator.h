#pragma once
#include <string>
#include <vector>
#include <memory>  // For std::shared_ptr
#include <unordered_map>
#include <queue>
#include <ctime>
#include "Process.h"
 
 typedef std::string String;

class PagingAllocator
{
public:
	PagingAllocator();
	PagingAllocator(size_t maxMemorySize, size_t mem_per_frame);
	~PagingAllocator();
	void initialize(size_t maxMemorySize, size_t mem_per_frame);
	static PagingAllocator* getInstance();

	void* allocate(std::shared_ptr<Process> process);
	void deallocate(String process);
	void visualizeMemory() const;
	
	void removeProcessFromBS(String processName);
	void saveProcessToBS(String process);
	String removeOldestEntry();

	size_t getNumPagedIn();
	size_t getNumPagedOut();

private:
	PagingAllocator& operator = (const PagingAllocator&);
	static PagingAllocator* sharedInstance;

	size_t maxMemorySize;
	size_t numFrames;
	size_t numPagedIn;
	size_t numPagedOut;
	std::unordered_map<size_t, std::pair<String, time_t>> frameMap;
	std::queue<size_t> freeFrameList;

	size_t allocateFrames(size_t numFrames, String processName);
	void deallocateFrames(size_t numFrames, size_t frameIndex);
};