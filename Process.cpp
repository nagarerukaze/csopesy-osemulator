#include "Process.h"

Process::Process() {}

Process::Process(std::string name, int linesOfCode) {
    this->name = name;
    this->currInstructionLine = 1;
    this->linesOfCode = linesOfCode;
    this->timeCreated = std::time(nullptr);

    std::ofstream outfile(this->name);

    outfile << "Process name: " << name << std::endl;
    outfile << "Logs:" << std::endl << std::endl;
}

// PRINT command
/*
    Creates a text file where all its
    associated print commands are written,
    with the timestamp of when it was executed
    by the CPU and the CPU core that executed it.
*/
void Process::printToTextFile() {
    std::ofstream outfile(this->name);

    outfile << "(" << this->getTimestamp() << ")" << name << " " << "Core: " << this->cpuCoreID << " " << "Hello World" << std::endl;

    // Execution time, core, command output (print)
    //outfile << execution tiime << this->cpuCoreID << commandOutput;

    this->nextLine();
}

void Process::draw() const {

    system("cls");

    std::cout << "Process name: " << this->getName() << std::endl;
    std::cout << "Current line of instruction / Total line of instruction: " << this->getCurrInstructionLine() << " / " << this->getLinesOfCode() << std::endl;
    //std::cout << "Timestamp: " << this->getTimestamp() << std::endl;

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

void Process::nextLine() {
    this->currInstructionLine += 1;
}

std::string Process::getName() const {
    return this->name;
}

Process::ProcessState Process::getState() const {
    return this->currentState;
}

bool Process::isFinished() const {
    if (currentState == ProcessState::TERMINATED) {
        return true;
    }

    return false;
}

void Process::terminate() {
    this->currentState = ProcessState::TERMINATED;
}

int Process::getCurrInstructionLine() const {
    return this->currInstructionLine;
}

int Process::getLinesOfCode() const {
    return this->linesOfCode;
}

int Process::getCPUCoreID() const {
    return this->cpuCoreID;
}

void Process::setCPUCoreID(int cpuCoreID) {
    this->cpuCoreID = cpuCoreID;
}


std::string Process::getTimestamp() const {
    std::tm timeInfo;
    localtime_s(&timeInfo, &timeCreated);
    std::ostringstream oss;
    oss << std::put_time(&timeInfo, "%m/%d/%Y, %I:%M:%S %p");

    return oss.str();
}

//void Process::setTimeCreated() {
//    this->timeCreated = this->getTimestamp();
//}

