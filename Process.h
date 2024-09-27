#pragma once
#include <string>
#include <ctime>
#include <iostream>

class Process
{
public:
	~Process() = default;
	Process(std::string name, int linesOfCode);


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

