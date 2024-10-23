#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <fstream>
#include <sstream>
#include <stdlib.h> 
#include <conio.h>

#include "ProcessManager.h"

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
	void schedulerTest();
	void schedulerStop();
	void reportUtil();

private:
	/*

	Singleton stuff

	*/
	ConsoleManager();
	ConsoleManager(const ConsoleManager&);
	ConsoleManager& operator = (const ConsoleManager&);
	~ConsoleManager() = default;
	static ConsoleManager* sharedInstance;

};

