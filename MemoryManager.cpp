#include "MemoryManager.h"
#include <iostream>
#include <unordered_map>

MemoryManager::MemoryManager() {}

MemoryManager::MemoryManager(size_t maximumSize, size_t memPerProc) {
    this->maximumSize = maximumSize;
    this->allocatedSize = 0;
    this->memPerProc = memPerProc;
    memory.resize(maximumSize);  // Resize memory to the given maximum size
    for (size_t i = 0; i < maximumSize / memPerProc; i++) {
        this->strProcessesInMemory.push_back(".");
    }
    initializeMemory();
}

MemoryManager* MemoryManager::sharedInstance = nullptr;

MemoryManager::~MemoryManager() {
    memory.clear();
}

void MemoryManager::initialize(size_t maximumSize, size_t memPerProc) {
    sharedInstance = new MemoryManager(maximumSize, memPerProc);
}


MemoryManager* MemoryManager::getInstance() {
    if (sharedInstance == nullptr) {
        sharedInstance = new MemoryManager;  // Ensure the instance is created
    }
    return sharedInstance;
}

// Allocate memory for a process
void* MemoryManager::allocate(size_t size, String process) {
    size_t index = 0;
    // Find the first available block that can accommodate the process
    for (size_t i = 0; i < maximumSize - size + 1; ++i) {
        if (!allocationMap[i] && canAllocateAt(i, size)) {
            allocateAt(i, size);
            // TODO: assign process name to strProcessesInMemory
            //////////////////////////////////////////////////
            if (i != 0) {
                index = i / size;
            }
            this->strProcessesInMemory[index] = process;
            //////////////////////////////////////////////////
            return &memory[i];
        }
    }

    // No available block found, return nullptr
    return nullptr;
}

void MemoryManager::deallocate(void* ptr, size_t size, String process) {
    size_t index = static_cast<char*>(ptr) - &memory[0];
    size_t i = 0;

    // Check if the index exists and if it's allocated
    if (allocationMap[index]) {
        deallocateAt(index, size);  // Deallocate the block starting from this index
        if (index != 0) {
            i = index / size;
        }
        this->strProcessesInMemory[i] = ".";
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
    allocatedSize -= size;
}

size_t MemoryManager::getMaximumMemory() {
    return this->maximumSize;
}

void MemoryManager::printMemory(long long qq) {
    std::stringstream filename;
    filename << "memory_stamp_" << qq << ".txt";
    std::ofstream myfile(filename.str());

    if (myfile.is_open())
    {
        myfile << "Timestamp: " << this->getCurrentTime() << "\n";
        myfile << "Number of processes in memory: " << this->strProcessesInMemory.size() << " \n";
        myfile << "Total external fragmentation in KB: " << (this->maximumSize - this->allocatedSize) << "\n";
        myfile << "\n-----end----- = " << this->maximumSize << "\n";
        this->printASCIIMemory(myfile);
        myfile << "\n----start---- = 0\n";
    }
    else {
        std::cout << "Unable to open file. Report was not successfully generated." << std::endl;
    }
}

/*
    Prints the ASCII printout of the memory in an indicated text file.

    For each process in memory, print its:
        (1) upper limit,
        (2) name, and
        (3) lower limit.
*/
void MemoryManager::printASCIIMemory(std::ofstream& outFile) {
    if (!this->strProcessesInMemory.empty()) {
        for (size_t i = (maximumSize / memPerProc) - 1; i < 0; i--) {
            if (this->strProcessesInMemory[i] != ".") {
                outFile << "\n" << (i * (this->memPerProc + 1)) << "\n" // Upper limit
                    << this->strProcessesInMemory[i] // Process name
                    << "\n" << (i * this->memPerProc) << std::endl; // Lower limit
            }
        }
    }
}

/*

    Returns the date and time now.
    
*/
String MemoryManager::getCurrentTime() {

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time;

    // For Windows
    localtime_s(&local_time, &now_time);

    // For Mac
    //localtime_r(&now_time, &local_time);

    // Format the time as a string
    std::ostringstream oss;
    oss << std::put_time(&local_time, "%d-%m-%Y %H:%M:%S");
    return oss.str();
}

