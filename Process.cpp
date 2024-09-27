#include "Process.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <string>

Process::Process() {}

Process::Process(std::string name, int linesOfCode) {
    this->name = name;
    this->currInstructionLine = 1;
    this->linesOfCode = linesOfCode;
    this->timeCreated = std::time(nullptr);
}

void Process::draw() const {

    system("cls");

    std::cout << "Process name: " << this->getName() << std::endl;
    std::cout << "Current line of instruction / Total line of instruction: " << this->getCurrInstructionLine() << " / " << this->getLinesOfCode() << std::endl;
    std::cout << "Timestamp: " << this->getTimestamp() << std::endl;

    std::string input;

    while (input != "exit") {
        // Get the input string from the user
        std::cout << "Enter command: ";
        std::getline(std::cin, input);

        // Create a stringstream to split the input
        std::stringstream ss(input);
        std::string word;
        std::vector<std::string> command;

        // Split the input string by spaces and store the words in the array
        while (ss >> word) {
            command.push_back(word);
        }

        if (command[0] != "exit") {
            std::cout << "Invalid command." << std::endl;
        }
    }


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