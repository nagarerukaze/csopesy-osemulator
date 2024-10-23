#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <fstream>
#include <sstream>
#include <stdlib.h> 
#include <conio.h>
#include <windows.h>

#include <thread>
#include <chrono>

#include "ProcessManager.h"

typedef std::string String;

class ConsoleManager
{
public:
	// Singleton stuff
	static void initializeConsole();
	static ConsoleManager* getInstance();

	void startCPUCycle();
	void printHeader();
	void clear();
	bool initialize();
	void screen(String command);
	void schedulerTest();
	void schedulerStop();
	void reportUtil();

	void setCursorPosition(int x, int y);
	void printAtPosition(int x, int y, const String& text);

	bool getIsRunning() const;
	int getCPUCycle() const;

	void stopRunning();
private:
	/*

	Singleton stuff

	*/
	ConsoleManager();
	ConsoleManager(const ConsoleManager&);
	ConsoleManager& operator = (const ConsoleManager&);
	~ConsoleManager() = default;
	static ConsoleManager* sharedInstance;

	bool isRunning;
	int cpuCycles;
	std::thread cpuThread;
	std::thread schedulerTestThread;
};

