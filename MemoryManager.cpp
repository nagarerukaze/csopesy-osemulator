#include "MemoryManager.h"
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <memory>

#include "CPUScheduler.h"
#include "CPUWorker.h"
#include "ProcessManager.h"

MemoryManager::MemoryManager() : maximumSize(0), allocatedSize(0) {}

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
void* MemoryManager::allocate(size_t size, String process) {
    size_t index = 0;
    // Find the first available block that can accommodate the process
    for (size_t i = 0; i < maximumSize - size + 1; ++i) {
        if (!allocationMap[i] && canAllocateAt(i, size)) {
            allocateAt(i, size, process);
            processOrder[process] = std::time(nullptr);
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
        processOrder.erase(process);
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

void MemoryManager::allocateAt(size_t index, size_t size, String process) {
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

void MemoryManager::displayRunningProcsAndMemUsage() {

    std::vector<CPUWorker*> workers = CPUScheduler::getInstance()->getCPUWorkers();

    for (const auto& worker : workers) {
        if (worker->getProcess() != nullptr) {
            std::shared_ptr<Process> process = worker->getProcess();

            if (process->getState() == Process::ProcessState::RUNNING) {
                std::cout << process->getName() << "\t"
                    << process->getProcessMemUsage() << "MiB" << std::endl;
            }
        }
    }
}

size_t MemoryManager::getAllocatedSize() {
    return this->allocatedSize;
}

size_t MemoryManager::getMaximumMemory() {
    return this->maximumSize;
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

    // For Mac/Linux
    // localtime_r(&now_time, &local_time);

    // Format the time as a string with AM/PM
    std::ostringstream oss;
    oss << std::put_time(&local_time, "%d-%m-%Y %I:%M:%S %p");  // %I for 12-hour clock, %p for AM/PM

    return oss.str();
}


void MemoryManager::removeProcessFromBS(String processName) {
    std::ifstream inFile("backing_store.txt");  // Read
    std::ofstream tempFile("temp.txt"); // Write

    tempFile.open("temp.txt", std::ofstream::out);

    if (!inFile.is_open()) {
        std::cerr << "Failed to open backing store for reading.\n";
        return;
    }

    if (!tempFile.is_open()) {
        std::cerr << "Failed to open temporary file for writing.\n";
        std::perror("Error");
        return;
    }

    String line;
    bool processFound = false;

    while (std::getline(inFile, line)) {
        if (line.empty()) {
            break;
        }

        if (line.find(processName) == std::string::npos) {
            tempFile << line << "\n";
        }
        else {
            processFound = true;
        }
    }

    inFile.close();
    tempFile.close();

    // If the process was found, replace the original file with the temporary file
    if (processFound) {
        std::remove("backing_store.txt"); // Delete the original file
        if (std::rename("temp.txt", "backing_store.txt") != 0) {
            std::perror("Error renaming file");
        }

        // std::cout << "Process " << processName << " removed from the backing store.\n";
    }
    else {
        std::remove("temp.txt"); // Clean up the temporary file if not used
        // std::cout << "Process " << processName << " not found in the backing store.\n";
    }
}


void MemoryManager::saveProcessToBS(void* memoryPointer, size_t memRequired, String name) {
    std::ofstream outFile("backing_store.txt", std::ios::app); // Append mode
    // deallocate(memoryPointer, memRequired, name);

    if (outFile.is_open()) {
        outFile << name << "\n";
        outFile.close();
    }
}

//String MemoryManager::removeOldestEntry() {
//    if (processOrder.empty()) {
//        return "";
//    }
//
//    auto oldest = std::min_element(
//        processOrder.begin(), processOrder.end(),
//        [](const auto& a, const auto& b) { return a.second < b.second; }
//    );
//
//    String oldestProcessName = oldest->first;
//    processOrder.erase(oldest);
//    return oldestProcessName;
//}

std::shared_ptr<Process> MemoryManager::removeOldestEntry() {
    if (processOrder.empty()) {
        return nullptr; // No process to remove
    }

    auto oldest = processOrder.end();
    std::shared_ptr<Process> oldestProcess = nullptr;

    for (auto it = processOrder.begin(); it != processOrder.end(); ++it) {
        std::lock_guard<std::mutex> lock(mtx);
        // Fetch the process using its name
        auto process = ProcessManager::getInstance()->findProcess(it->first);

        // Skip processes that are TERMINATED or RUNNING
        if (process && process->getState() != Process::ProcessState::TERMINATED &&
            process->getState() != Process::ProcessState::RUNNING) {

            // Check if this is the oldest entry
            if (oldest == processOrder.end() || it->second < oldest->second) {
                oldest = it;
                oldestProcess = process;
            }
        }
    }

    if (oldest == processOrder.end()) {
        return nullptr; // No valid process found
    }

    // Remove the oldest process from processOrder
    // deallocate(oldestProcess->getMemoryPointer(), oldestProcess->getMemoryRequired(), oldestProcess->getName());
    return oldestProcess;
}
