#include "MemoryManager.h"
#include <iostream>

MemoryManager::MemoryManager() {}

MemoryManager::MemoryManager(size_t totalMemory, size_t frameSize, size_t memPerProc) {
    this->totalMemory = totalMemory;
    this->frameSize = frameSize;
    this->memPerProc = memPerProc;
    this->numFrames = totalMemory / frameSize;
    this->memoryBlocks.resize(numFrames, "");  // Initialize all frames as free (empty string)
}

//MemoryManager::MemoryManager(const MemoryManager&) {}

//MemoryManager* MemoryManager::sharedInstance = nullptr;

void MemoryManager::initialize(size_t totalMemory, size_t frameSize, size_t memPerProc) {
    sharedInstance = new MemoryManager(totalMemory, frameSize, memPerProc);
}

MemoryManager* MemoryManager::getInstance() {
    if (sharedInstance == NULL)
    {
        sharedInstance = new MemoryManager;
    }

    return sharedInstance;
}

bool MemoryManager::allocateMemory(Process* process) {
    size_t requiredFrames = memPerProc / frameSize;
    const std::string& processName = process->getName();

    for (size_t i = 0; i <= memoryBlocks.size() - requiredFrames; ++i) {
        bool canAllocate = true;

        // Check if there's enough contiguous free frames starting from `i`
        for (size_t j = 0; j < requiredFrames; ++j) {
            if (!memoryBlocks[i + j].empty()) {  // Frame is already occupied
                canAllocate = false;
                break;
            }
        }

        // Allocate memory if a suitable space is found
        if (canAllocate) {
            for (size_t j = 0; j < requiredFrames; ++j) {
                memoryBlocks[i + j] = processName;
            }
            // Set the memory range for the process
            process->setMemoryRange(i, i + requiredFrames - 1);
            return true;
        }
    }

    std::cerr << "Memory allocation failed for process " << processName << ": Not enough contiguous memory available.\n";
    return false;  // No suitable memory found
}

void MemoryManager::freeMemory(Process* process) {
    size_t startFrame = process->getStartFrame();
    size_t endFrame = process->getEndFrame();

    for (size_t i = startFrame; i <= endFrame; ++i) {
        this->memoryBlocks[i] = "";  // Clear the frame by setting it to an empty string
    }
}

void MemoryManager::printMemory() const {
    std::cout << "Memory Layout:" << std::endl;
    for (size_t i = 0; i < memoryBlocks.size(); ++i) {
        if (this->memoryBlocks[i].empty()) {
            std::cout << ". ";  // Indicate a free frame
        }
        else {
            std::cout << memoryBlocks[i] << " ";  // Print process name in occupied frame
        }

        if ((i + 1) % 16 == 0) {  // Newline for every 16 frames for readability
            std::cout << std::endl;
        }
    }
}

size_t MemoryManager::calculateExternalFragmentation() const {
    size_t fragmentation = 0;

    for (const auto& block : this->memoryBlocks) {
        if (block.empty()) {
            fragmentation += frameSize;  // Count every empty frame
        }
    }

    return fragmentation / 1024;  // Convert to KB for the fragmentation report
}

// Generate memory snapshot
void MemoryManager::generateMemorySnapshot(int quantumCycle) const {
    // Create the file with the quantum cycle in the filename
    std::ofstream snapshotFile("memory_stamp_" + std::to_string(quantumCycle) + ".txt");

    if (!snapshotFile.is_open()) {
        std::cerr << "Unable to open file for writing memory snapshot." << std::endl;
        return;
    }

    // Timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    snapshotFile << "Timestamp: " << std::put_time(std::localtime(&currentTime), "%m/%d/%Y %I:%M:%S%p") << "\n";

    // Number of processes in memory
    size_t processCount = 0;
    for (const auto& block : memoryBlocks) {
        if (!block.empty()) {
            processCount++;
        }
    }
    snapshotFile << "Number of processes in memory: " << processCount << "\n";

    // External fragmentation
    snapshotFile << "Total external fragmentation in KB: " << std::fixed << std::setprecision(2)
        << calculateExternalFragmentation() / 1024.0 << "\n\n";

    // Memory layout ASCII printout
    snapshotFile << "----end---- = 16384\n\n";

    // Print memory layout details in requested format
    for (const auto& block : memoryBlocks) {
        // Each block has an address, process ID (or "Empty"), and size
        snapshotFile << block.endAddress << "\n";
        snapshotFile << (block.empty() ? "Empty" : block.processID) << "\n";
        snapshotFile << block.size << "\n\n";
    }

    snapshotFile << "----start---- = 0\n";

    snapshotFile.close();
    std::cout << "Memory snapshot saved to memory_stamp_" << quantumCycle << ".txt" << std::endl;
}




