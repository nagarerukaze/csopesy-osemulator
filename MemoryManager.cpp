#include "MemoryManager.h"
#include <iostream>

MemoryManager::MemoryManager(size_t totalMemory, size_t frameSize, size_t memPerProc) {
    this->totalMemory = totalMemory;
    this->frameSize = frameSize;
    this->memPerProc = memPerProc;
    this->numFrames = totalMemory / frameSize;
    this->memoryBlocks.resize(numFrames, "");  // Initialize all frames as free (empty string)
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

