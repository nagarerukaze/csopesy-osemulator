#include "Process.h"
#include <ctime>
#include <iomanip>
#include <sstream>

Process::Process() {

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