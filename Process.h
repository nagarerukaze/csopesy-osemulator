#pragma once
#include <string>
#include <ctime>

class Process
{
public:
	static void initialize();
	std::string getName() const;
	int getCurrInstructionLine() const;
	int getLinesOfCode() const;
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

