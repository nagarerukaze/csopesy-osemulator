#include "Process.h"

Process::Process() {}

Process::Process(String name, long long totalLinesOfCode) {
	this->name = name;
	this->currentInstructionLine = 0;
	this->totalLinesOfCode = totalLinesOfCode;
	this->timeCreated = time(0);
    this->currentState = Process::READY;
}

void Process::printInfo() {
    std::cout << "Process: " << this->getName() << std::endl;
    std::cout << std::endl;
    
    if (this->getState() == TERMINATED) {
        std::cout << "Finished!" << std::endl;
    }
    else {
        std::cout << "Current instruction line: " << this->getCurrentInstructionLine() << std::endl;
        std::cout << "Lines of code: " << this->getTotalLinesOfCode() << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Timestamp: " << this->getTimestamp() << std::endl;
    std::cout << std::endl;
}

void Process::draw() {
    // For Windows
    system("cls");

    // For Mac
    system("clear");
    
    printInfo();
    bool isRunning = true;
    String command;

    while (isRunning) {
        std::cout << "Enter command: ";
        std::getline(std::cin, command);

        if (command == "process-smi") {
            std::cout << std::endl;
            printInfo();
        }
        else if (command == "exit") {
            isRunning = false;
        }
        else {
            std::cout << "Unknown command." << std::endl;
        }
    }
}

void Process::nextLine() {
    this->currentInstructionLine += 1;
}

// getters and setters
String Process::getName() const {
    return this->name;
}

long long Process::getCurrentInstructionLine() const {
    return this->currentInstructionLine;
}

long long Process::getTotalLinesOfCode() const {
    return this->totalLinesOfCode;
}

String Process::getTimestamp() {
    tm ltm;
    
    // For Windows
    //localtime_s(&ltm, &(this->timeCreated));

    // For Mac
     localtime_r(&(this->timeCreated), &ltm);
    
    std::ostringstream oss;
    oss << std::put_time(&ltm, "%m/%d/%Y %I:%M:%S%p");
    return oss.str();
}

Process::ProcessState Process::getState() const {
    return this->currentState;
}

void Process::setState(Process::ProcessState state) {
    this->currentState = state;
}

int Process::getCPUCoreID() {
   return this->cpuCoreID;
}

void Process::setCPUCoreID(int cpuCoreID) {
   this->cpuCoreID = cpuCoreID;
}
