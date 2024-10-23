#include "Process.h"

Process::Process() {}

Process::Process(String name, int totalLinesOfCode) {
	this->name = name;
	this->currentInstructionLine = 0; // TODO: Not sure if this should be 1
	this->totalLinesOfCode = totalLinesOfCode;
	this->timeCreated = time(0);
    this->currentState = READY;
}

void Process::printInfo() {
    std::cout << "Process: " << this->getName() << std::endl;
    std::cout << std::endl;
    std::cout << "Current instruction line: " << this->getCurrentInstructionLine() << std::endl;
    
    if (this->getState() == TERMINATED) {
        std::cout << "Finished!" << std::endl;
    }
    else {
        std::cout << "Lines of code: " << this->getTotalLinesOfCode() << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Timestamp: " << this->getTimestamp() << std::endl;
    std::cout << std::endl;
}

void Process::draw() {
    system("cls");
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

int Process::getCurrentInstructionLine() const {
    return this->currentInstructionLine;
}

int Process::getTotalLinesOfCode() const {
    return this->totalLinesOfCode;
}

String Process::getTimestamp() {
    tm ltm;
    localtime_s(&ltm, &(this->timeCreated));
    std::ostringstream oss;
    oss << std::put_time(&ltm, "%m/%d/%Y, %I:%M:%S %p");
    return oss.str();
}

Process::ProcessState Process::getState() const {
    return this->currentState;
}

void Process::setState(ProcessState state) {
    this->currentState = state;
}

//void Process::printCommand(std::ofstream& outfile) {
//    outfile << "(" << this->getTimestamp() << ") " << name << " " << "Core: " << this->cpuCoreID << " " << "Hello World" << std::endl;
//    this->nextLine();
//}
//
// PRINT command

//
//bool Process::isFinished() const {
//    if (currentState == ProcessState::TERMINATED) {
//        return true;
//    }
//
//    return false;
//}
//
//
//int Process::getCPUCoreID() const {
//    return this->cpuCoreID;
//}
//
//void Process::setCPUCoreID(int cpuCoreID) {
//    this->cpuCoreID = cpuCoreID;
//}
//
//void Process::setTimeCreated() {
//    this->timeCreated = this->getTimestamp();
//}
//
