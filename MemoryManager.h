#pragma once
#include <vector>
#include <unordered_map>
#include "Process.h"

typedef std::string String;

class MemoryManager
{
public:
	void initialize(size_t maximumSize, size_t allocatedSize);
	static MemoryManager* getInstance();


	void* allocate(size_t size);
	void deallocate(void* ptr);
	String visualizeMemory();
private:

	MemoryManager();
	MemoryManager(size_t totalMemory, size_t allocatedSize);
	MemoryManager(const MemoryManager&) = delete;
	MemoryManager& operator = (const MemoryManager&);
	~MemoryManager();
	static MemoryManager* sharedInstance;

	size_t maximumSize;
	size_t allocatedSize;
	std::vector<char> memory;
	std::unordered_map<size_t, bool> allocationMap;

	void initializeMemory();
	bool canAllocateAt(size_t index, size_t size) const;
	void allocateAt(size_t index, size_t size);
	void deallocateAt(size_t index);
};

