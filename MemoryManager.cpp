#include "MemoryManager.h"
#include <iostream>
#include <unordered_map>

MemoryManager::MemoryManager() {}

MemoryManager::MemoryManager(size_t maximumSize, size_t allocatedSize) {
    this->maximumSize = maximumSize;
    this->allocatedSize = allocatedSize;
    memory.resize(maximumSize);  // Resize memory to the given maximum size
    initializeMemory();
}

MemoryManager* MemoryManager::sharedInstance = nullptr;

MemoryManager::~MemoryManager() {
    memory.clear();
}

void MemoryManager::initialize(size_t maximumSize, size_t allocatedSize) {
    if (sharedInstance == nullptr) {
        sharedInstance = new MemoryManager(maximumSize, allocatedSize);
    }
}

MemoryManager* MemoryManager::getInstance() {
    if (sharedInstance == nullptr) {
        sharedInstance = new MemoryManager;  // Ensure the instance is created
    }
    return sharedInstance;
}

void* MemoryManager::allocate(size_t size) {
    // Find the first available block that can accommodate the process
    for (size_t i = 0; i < maximumSize - size + 1; ++i) {
        bool canAllocate = true;

        // Check if all blocks from i to i+size are available
        for (size_t j = i; j < i + size; ++j) {
            if (allocationMap.find(j) != allocationMap.end() && allocationMap[j]) {
                canAllocate = false;  // Found an allocated block, can't allocate
                break;
            }
        }

        if (canAllocate) {
            // Allocate memory by marking the blocks as allocated
            for (size_t j = i; j < i + size; ++j) {
                allocationMap[j] = true;
            }
            allocatedSize += size;
            return &memory[i];  // Return pointer to the allocated memory block
        }
    }

    // No available block found, return nullptr
    return nullptr;
}

void MemoryManager::deallocate(void* ptr) {
    size_t index = static_cast<char*>(ptr) - &memory[0];

    // Check if the index exists and if it's allocated
    if (allocationMap.find(index) != allocationMap.end() && allocationMap[index]) {
        deallocateAt(index);  // Deallocate the block starting from this index
    }
}

String MemoryManager::visualizeMemory() {
    return String(memory.begin(), memory.end());
}

void MemoryManager::initializeMemory() {
    std::fill(memory.begin(), memory.end(), '.');
    allocationMap.clear();  // Start with an empty allocation map
}

bool MemoryManager::canAllocateAt(size_t index, size_t size) const {
    // Check if the memory block is large enough
    return (index + size <= maximumSize);
}

void MemoryManager::deallocateAt(size_t index) {
    size_t size = 0;

    // Determine the size of the allocated block by checking consecutive allocations
    while (allocationMap.find(index + size) != allocationMap.end() && allocationMap[index + size]) {
        ++size;
    }

    // Mark the memory block as deallocated by removing entries from allocationMap
    for (size_t i = index; i < index + size; ++i) {
        allocationMap.erase(i);  // Remove the allocated entry from the map
    }

    allocatedSize -= size;  // Decrease allocated size
}
