#pragma once
#include <string>
#include <ctime>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <fstream>  

class Process
{
public:
	Process();
	~Process() = default;
	Process(std::string name, int linesOfCode);


	void execute();

private:
	std::string name;
	int currInstructionLine;
	int linesOfCode;
	std::time_t timeCreated;

	void draw() const;
	std::string getName() const;
	int getCurrInstructionLine() const;
	int getLinesOfCode() const;
	std::string getTimestamp() const;

	friend class ProcessManager;
};

