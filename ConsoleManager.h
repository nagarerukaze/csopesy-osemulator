#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <fstream>
#include <sstream>
#include <stdlib.h> 

#include <thread>
#include <chrono>

#include "ProcessManager.h"
#include "CPUScheduler.h"

typedef std::string String;

class ConsoleManager
{
public:
	// Singleton stuff
	static void initializeConsole();
	static ConsoleManager* getInstance();

	void printHeader();
	void clear();
	bool initialize();
	void screen(String command);
	void generateProcesses();
	void schedulerTest();
	void schedulerStop();
	void reportUtil();

	bool getIsRunning() const;
	long long getCPUCycle() const;

	void stopRunning();
private:
	// Singleton stuff
	ConsoleManager();
	ConsoleManager(const ConsoleManager&);
	ConsoleManager& operator = (const ConsoleManager&);
	~ConsoleManager() = default;
	static ConsoleManager* sharedInstance;

	bool isRunning;
	long long cpuCycles;
	std::thread cpuThread;
	std::thread schedulerTestThread;
	std::mutex mtx;
};

