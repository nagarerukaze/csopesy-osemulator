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

void PagingAllocator::deallocate(std::shared_ptr<Process> process) {
	String processName = process->getName();

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
}

size_t PagingAllocator::getNumPagedIn() {
	return this->numPagedIn;
}

size_t PagingAllocator::getNumPagedOut() {
	return this->numPagedOut;
}

