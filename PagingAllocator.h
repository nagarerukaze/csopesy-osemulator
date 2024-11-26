#pragma once
#include <string>
#include <vector>
#include <memory>  // For std::shared_ptr
#include <unordered_map>

#include "Process.h"
 
 typedef std::string String;

class PagingAllocator
{
public:
	  PagingAllocator(size_t maxMemorySize, size_t mem_per_frame);

	  void* allocate(std::shared_ptr<Process> process);
	  void deallocate(std::shared_ptr<Process> process);
	  void visualizeMemory() const;

private:
	size_t maxMemorySize;
	size_t numFrames;
	std::unordered_map<size_t, String> frameMap;
	std::vector<size_t> freeFrameList;

	size_t allocateFrames(size_t numFrames, String processName);
	void deallocateFrames(size_t numFrames, size_t frameIndex);


};