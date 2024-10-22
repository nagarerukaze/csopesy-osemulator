#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

typedef std::string String;

class ConsoleManager
{
public:
	// Singleton stuff
	static void initialize();
	static ConsoleManager* getInstance();

	void printHeader();
	void clear();
	void screen(String command);

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

