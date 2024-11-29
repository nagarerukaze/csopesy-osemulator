#include "PagingAllocator.h"

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

void PagingAllocator::removeProcessFromBS(String processName) {
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


void PagingAllocator::saveProcessToBS(String process) {
	std::ofstream outFile("backing_store.txt", std::ios::app); // Append mode
	deallocate(process);

	if (outFile.is_open()) {
		outFile << process << "\n";
		outFile.close();
	}
}

// Function to find and remove the entry with the oldest time_t, and return the process name
String PagingAllocator::removeOldestEntry() {
	std::pair<size_t, std::pair<String, time_t>> oldestEntry;
	bool isFirst = true;
	size_t oldestKey = 0;

	// Find the oldest entry
	for (const auto& entry : frameMap) {
		if (isFirst || entry.second.second < oldestEntry.second.second) {
			oldestEntry = entry;
			oldestKey = entry.first;  // store the key of the oldest entry
			isFirst = false;
		}
	}

	// If we found the oldest entry, remove it and return the name
	if (!isFirst) {
		String oldestName = oldestEntry.second.first;  // Extract the name to be removed
		frameMap.erase(oldestKey);  // Remove the oldest entry from the map
		return oldestName;
	}

	return "";  // Return empty string if no entry was found
}