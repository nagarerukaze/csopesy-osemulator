#include "Process.h"

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
	
}