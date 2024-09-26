#include "Process.h"
#include <ctime>
#include <iomanip>
#include <sstream>

Process::Process() {}

Process::Process(std::string name, int currInstructionLine, int linesOfCode) {
    this->name = name;
    this->currInstructionLine = currInstructionLine;
    this->linesOfCode = linesOfCode;
    this->timeCreated = std::time(nullptr);
}

std::string Process::getName() const {
	return name;
}

int Process::getCurrInstructionLine() const {
	return currInstructionLine;
}

int Process::getLinesOfCode() const {
	return linesOfCode;
}

std::string Process::getTimestamp() const {
    std::tm* timeInfo = std::localtime(&timeCreated);
    std::ostringstream oss;

    oss << std::put_time(timeInfo, "%m/%d/%Y, %I:%M:%S %p");

    return oss.str();
}