#include "MemoryManager.h"
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <memory>

#include "CPUScheduler.h"
#include "CPUWorker.h"

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
            // TODO: assign process name to strProcessesInMemory
            //////////////////////////////////////////////////
            /*if (i != 0) {
                index = i / size;
            }*/
            // this->strProcessesInMemory[index] = process;
            //////////////////////////////////////////////////
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
        // this->strProcessesInMemory[i] = ".";
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
    // MemoryEntry entry = { process, std::time(nullptr) };
    for (size_t i = index; i < index + size; ++i) {
        allocationMap[i] = true;  // Mark each block as allocated
        // memoryDetails[i] = entry;
    }
    allocatedSize += size;
}

void MemoryManager::deallocateAt(size_t index, size_t size) {
    for (size_t i = index; i < index + size; ++i) {
        allocationMap[i] = false;  // Mark each block as allocated
        // memoryDetails[i] = { "", 0 };
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
                    << "<memory usage>" << "MiB" << std::endl;
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

//
//void MemoryManager::printMemory(long long qq) {
//
//    std::string folderPath = "reports";  // Specify your subfolder name here
//
//    // Ensure the subfolder exists
//    if (!std::filesystem::exists(folderPath)) {
//        std::filesystem::create_directory(folderPath);  // Create the subfolder if it doesn't exist
//    }
//
//    long long numberOfProcesses = 0;
//
//    if (!this->strProcessesInMemory.empty()) {
//        for (int i = (maximumSize / memPerProc) - 1; i >= 0; i--) {
//            if (this->strProcessesInMemory[i] != ".") {
//                numberOfProcesses++;
//            }
//        }
//    }
//
//    std::stringstream filename;
//    filename << folderPath << "/memory_stamp_" << qq << ".txt";
//    std::ofstream myfile(filename.str());
//
//    if (myfile.is_open())
//    {
//        myfile << "Timestamp: " << "(" << this->getCurrentTime() << ")" << "\n";
//
//        // should not be .size()
//        myfile << "Number of processes in memory: " << numberOfProcesses << " \n";
//        
//        myfile << "Total external fragmentation in KB: " << (this->maximumSize - this->allocatedSize) << "\n";
//        myfile << "\n-----end----- = " << this->maximumSize << "\n";
//        this->printASCIIMemory(myfile);
//        myfile << "\n----start---- = 0\n";
//    }
//    else {
//        std::cout << "Unable to open file. Report was not successfully generated." << std::endl;
//    }
//}

///*
//    Prints the ASCII printout of the memory in an indicated text file.
//
//    For each process in memory, print its:
//        (1) upper limit,
//        (2) name, and
//        (3) lower limit.
//*/
//void MemoryManager::printASCIIMemory(std::ofstream& outFile) {
//    if (!this->strProcessesInMemory.empty()) {
//        for (int i = (maximumSize / memPerProc) - 1; i >= 0; i--) {
//            if (this->strProcessesInMemory[i] != ".") {
//                outFile << "\n" << ((i + 1) * this->memPerProc) << "\n" // Upper limit
//                    << this->strProcessesInMemory[i] // Process name
//                    << "\n" << (i * this->memPerProc) << std::endl; // Lower limit
//            }
//        }
//    }
//}

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
    std::ifstream inFile("backing_store.txt");
    std::ofstream tempFile("temp_backing_store.txt");

    if (!inFile.is_open()) {
        std::cerr << "Failed to open backing store for reading.\n";
        return;
    }

    if (!tempFile.is_open()) {
        std::cerr << "Failed to open temporary file for writing.\n";
        return;
    }

    std::string line;
    bool processFound = false;

    // Iterate through each line in the original file
    while (std::getline(inFile, line)) {
        // Skip the line that contains the process to remove
        if (line.find(processName) == std::string::npos) {
            // If the line doesn't contain the process name, copy it to the temp file
            tempFile << line << "\n";
        }
        else {
            processFound = true;

        }
    }

    // Close the input and temporary files
    inFile.close();
    tempFile.close();

    // If the process was found, replace the original file with the temporary file
    if (processFound) {
        std::remove("backing_store.txt"); // Delete the original file
        std::rename("temp_backing_store.txt", "backing_store.txt"); // Rename the temp file to the original name
    }
    else {
        std::remove("temp_backing_store.txt"); // Clean up the temporary file if not used
    }
}


void MemoryManager::saveProcessToBS(void* memoryPointer, size_t memRequired, String name) {
    std::ofstream outFile("backing_store.txt", std::ios::app); // Append mode
    deallocate(memoryPointer, memRequired, name);

    if (outFile.is_open()) {
        outFile << name << "\n";
        outFile.close();
    }
}
String MemoryManager::removeOldestEntry() {
    if (processOrder.empty()) {
        return ""; // Return empty string if the map is empty
    }

    // Find the oldest process
    auto oldest = std::min_element(
        processOrder.begin(), processOrder.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; }
    );

    // Save the name of the oldest process
    String oldestProcessName = oldest->first;

    // Remove the oldest entry from the map
    processOrder.erase(oldest);

    // Return the name of the removed process
    return oldestProcessName;
}