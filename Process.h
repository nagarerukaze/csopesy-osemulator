#pragma once
#include <string>
#include <ctime>

class Process
{
public:
	static void initialize();
	std::string getName();
	std::string getCurrInstructionLine();
	std::string getLinesOfCode();
	std::string getTimestamp() const;
private:
	Process();
	~Process() = default;
	Process(std::string name, int currInstructionLine, int linesOfCode);
	std::string name;
	int currInstructionLine;
	int linesOfCode;
	std::time_t timeCreated;
};

