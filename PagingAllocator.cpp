#include "PagingAllocator.h"
#include "ProcessManager.h"

PagingAllocator::PagingAllocator() {}

PagingAllocator::PagingAllocator(size_t maxMemorySize, size_t mem_per_frame)
	: maxMemorySize(maxMemorySize), numFrames(maxMemorySize / mem_per_frame) {
	// Initialize Free Frame List
	for (size_t i = 0; i < numFrames; ++i) {
		freeFrameList.push(i);
	}
	this->numPagedIn = 0;
	this->numPagedOut = 0;
}

PagingAllocator* PagingAllocator::sharedInstance = nullptr;

void PagingAllocator::initialize(size_t maxMemorySize, size_t mem_per_frame) {
	sharedInstance = new PagingAllocator(maxMemorySize, mem_per_frame);
}

PagingAllocator* PagingAllocator::getInstance() {
	if (sharedInstance == nullptr) {
		sharedInstance = new PagingAllocator;  // Ensure the instance is created
	}
	return sharedInstance;
}

void* PagingAllocator::allocate(std::shared_ptr<Process> process) {
	String processName = process->getName();
	size_t numFramesNeeded = process->getNumPages();
	if (numFramesNeeded > freeFrameList.size()) {
		//std::cout << "Allocate failed." << std::endl;
		// Memory Failed
		return nullptr;
	}

	//std::cout << "Allocate success." << std::endl;

	// Allocate frames for the process
	size_t frameIndex = allocateFrames(numFramesNeeded, processName);
	return reinterpret_cast<void*>(frameIndex + 1); // simply bc 0 makes it == nullptr
}

void PagingAllocator::deallocate(String processName) {
	// Find frames allocated to the process and deallocate
	auto it = std::find_if(frameMap.begin(), frameMap.end(),
		[processName](const auto& entry) { return entry.second.first == processName; });

	while (it != frameMap.end()) {
		size_t frameIndex = it->first;
		deallocateFrames(1, frameIndex);
		it = std::find_if(frameMap.begin(), frameMap.end(),
			[processName](const auto& entry) { return entry.second.first == processName; });
	}
}

void PagingAllocator::visualizeMemory() const {
	std::cout << "Memory Visualization:\n";
	for (size_t frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
		auto it = frameMap.find(frameIndex);
		if (it != frameMap.end()) {
			std::cout << "Frame " << frameIndex << " -> Process " << it->second.first << "\n";
		}
		else {
			std::cout << "Frame " << frameIndex << " -> Free\n";
		}
	}
	std::cout << "-----------------------\n";
}

size_t PagingAllocator::allocateFrames(size_t numFrames, String processName) {
	size_t frameIndex = freeFrameList.front(); // TODO: Is this .front() not .back()?
	//std::cout << "FrameIndex: " << frameIndex << std::endl;
	time_t timeAdded = std::time(nullptr);
	// Map allocated frames to the process ID
	for (size_t i = 0; i < numFrames; ++i) {
		frameMap[frameIndex + i] = { processName, timeAdded };
		freeFrameList.pop();
	}

	this->numPagedIn++;

	return frameIndex;
}

void PagingAllocator::deallocateFrames(size_t numFrames, size_t frameIndex) {
	// Remove mapping of deallocated frames
	for (size_t i = 0; i < numFrames; ++i) {
		frameMap.erase(frameIndex + i);
	}

	for (size_t i = 0; i < numFrames; ++i) {
		freeFrameList.push(frameIndex + i);
	}

	this->numPagedOut++;
}

size_t PagingAllocator::getNumPagedIn() {
	return this->numPagedIn;
}

size_t PagingAllocator::getNumPagedOut() {
	return this->numPagedOut;
}

size_t PagingAllocator::getNumOfFrames() {
	return this->numFrames;
}
size_t PagingAllocator::getFreeFrames() {
	return this->freeFrameList.size();
}

void PagingAllocator::removeProcessFromBS(String processName) {
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


void PagingAllocator::saveProcessToBS(String process) {
	std::ofstream outFile("backing_store.txt", std::ios::app); // Append mode
	deallocate(process);

	if (outFile.is_open()) {
		outFile << process << "\n";
		outFile.close();
	}
}

// Function to find and remove the entry with the oldest time_t, and return the process name
std::shared_ptr<Process> PagingAllocator::removeOldestEntry() {
	std::pair<size_t, std::pair<String, time_t>> oldestEntry;
	bool isFirst = true;
	size_t oldestKey = 0;
	std::shared_ptr<Process> oldestProcess = nullptr;

	if (!frameMap.empty()) {
		// Find the oldest entry
		for (const auto& entry : frameMap) {
			std::shared_ptr<Process> process = ProcessManager::getInstance()->findProcess(entry.second.first);
			if (process->getState() != Process::ProcessState::TERMINATED && process->getState() != Process::ProcessState::RUNNING) {
				if (isFirst || entry.second.second < oldestEntry.second.second) {
					oldestEntry = entry;
					oldestProcess = process;
					isFirst = false;
					// std::cout << "Oldest entry updated" << std::endl;
					// std::cout << "State:" << process->getState() << std::endl;
				}
			}
		}
	}

	// If we found the oldest entry, remove it and return the name
	if (!isFirst) {
		String oldestName = oldestProcess->getName();  // Extract the name to be removed
		// std::cout << "Oldest process: " << oldestName << std::endl;
		// std::cout << "State:" << oldestProcess->getState() << std::endl;
		deallocate(oldestName);
		return oldestProcess;
	}

	return nullptr;  // Return empty string if no entry was found
}

