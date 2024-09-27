#include "Process.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdlib.h>

Process::Process(std::string name, int linesOfCode) {
    this->name = name;
    this->currInstructionLine = 1;
    this->linesOfCode = linesOfCode;
    this->timeCreated = std::time(nullptr);
}

void Process::draw() const {

    std::system("clear");

    std::cout << "Process name: " << this->getName() << std::endl;
    std::cout << "Current line of instruction / Total line of instruction: " << this->getCurrInstructionLine() << " / " << this->getLinesOfCode() << std::endl;
    std::cout << "Timestamp: " << this->getTimestamp() << std::endl;


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
    std::tm timeInfo;
    localtime_s(&timeInfo, &timeCreated);

    std::ostringstream oss;
    oss << std::put_time(&timeInfo, "%m/%d/%Y, %I:%M:%S %p");

    return oss.str();
}