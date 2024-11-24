#pragma once
#include <string>
#include <vector>
#include <memory>  // For std::shared_ptr

#include "Process.h"

class PagingAllocator
{
public: PagingAllocator(size_t maxMemorySize);

	  void* allocate(std::shared_ptr<Process> process);
	  void deallcoate(std::shared_ptr<Process> process);
	  void visualizeMemory() const;
};