#include "MemoryManager.h"
#include <iostream>
#include <unordered_map>

MemoryManager::MemoryManager() {}

MemoryManager::MemoryManager(size_t maximumSize) {
    this->maximumSize = maximumSize;
    this->allocatedSize = 0;
    memory.resize(maximumSize);  // Resize memory to the given maximum size
    initializeMemory();
}

MemoryManager* MemoryManager::sharedInstance = nullptr;

MemoryManager::~MemoryManager() {
    memory.clear();
}

void MemoryManager::initialize(size_t maximumSize) {
    sharedInstance = new MemoryManager(maximumSize);
}


MemoryManager* MemoryManager::getInstance() {
    if (sharedInstance == nullptr) {
        sharedInstance = new MemoryManager;  // Ensure the instance is created
    }
    return sharedInstance;
}

// Allocate memory for a process
void* MemoryManager::allocate(size_t size) {
    // Find the first available block that can accommodate the process
    for (size_t i = 0; i < maximumSize - size + 1; ++i) {
        if (!allocationMap[i] && canAllocateAt(i, size)) {
            allocateAt(i, size);
            return &memory[i];
        }
    }

    // No available block found, return nullptr
    return nullptr;
}

void MemoryManager::deallocate(void* ptr, size_t size) {
    size_t index = static_cast<char*>(ptr) - &memory[0];

    // Check if the index exists and if it's allocated
    if (allocationMap[index]) {
        deallocateAt(index, size);  // Deallocate the block starting from this index
    }
}

String MemoryManager::visualizeMemory() {
    // Update memory visualization based on allocationMap
    for (size_t i = 0; i < maximumSize; ++i) {
        // Check if the block at index i is allocated
        if (allocationMap.find(i) != allocationMap.end() && allocationMap[i]) {
            memory[i] = 'X';  // Mark allocated memory with 'X'
        }
        else {
            memory[i] = '.';  // Unallocated memory with '.'
        }
    }

    // Return a string representation of the memory (for visualization)
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

void MemoryManager::allocateAt(size_t index, size_t size) {
    for (size_t i = index; i < index + size; ++i) {
        allocationMap[i] = true;  // Mark each block as allocated
    }
    allocatedSize += size;
}



void MemoryManager::deallocateAt(size_t index, size_t size) {
    for (size_t i = index; i < index + size; ++i) {
        allocationMap[i] = false;  // Mark each block as allocated
    }
}

size_t MemoryManager::getMaximumMemory() {
    return this->maximumSize;
}