#include "Process.h"

Process::Process() {}

Process::Process(std::string name, int totalLinesOfCode) {
	this->name = name;
	this->currentInstructionLine = 0; // TODO: Not sure if this should be 1
	this->totalLinesOfCode = totalLinesOfCode;
	this->timeCreated = time(0);
}

void Process::printInfo() {
    std::cout << "Process: " << this->getName() << std::endl;
    std::cout << std::endl;
    std::cout << "Current instruction line: " << this->getCurrentInstructionLine() << std::endl;
    std::cout << "Lines of code: " << this->getTotalLinesOfCode() << std::endl;

    // TODO: If FINISHED, only print "FINISHED"
    std::cout << std::endl;
    std::cout << "Timestamp: " << this->getTimestamp() << std::endl;
    std::cout << std::endl;
}

void Process::draw() {
    system("cls");
    printInfo();
    bool isRunning = true;
    std::string command;

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
std::string Process::getName() const {
    return this->name;
}

int Process::getCurrentInstructionLine() const {
    return this->currentInstructionLine;
}

int Process::getTotalLinesOfCode() const {
    return this->totalLinesOfCode;
}

std::string Process::getTimestamp() {
    tm ltm;

    // Use localtime_s for thread-safe conversion
    localtime_s(&ltm, &(this->timeCreated));
    std::ostringstream oss;
    oss << std::put_time(&ltm, "%m/%d/%Y, %I:%M:%S %p"); // Format with AM/PM

    /*int hour = ltm->tm_hour;
    std::string ampm = (hour >= 12) ? "PM" : "AM";

    hour = (hour % 12);
    if (hour == 0) hour = 12;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << ltm->tm_mon + 1 << "/"
        << std::setfill('0') << std::setw(2) << ltm->tm_mday << "/"
        << (ltm->tm_year + 1900) << ", "
        << std::setfill('0') << std::setw(2) << hour << ":"
        << std::setfill('0') << std::setw(2) << ltm->tm_min << ":"
        << std::setfill('0') << std::setw(2) << ltm->tm_sec << " "
        << ampm;*/

    return oss.str();
}

//#include "Process.h"
//
//Process::Process() {}
//
//Process::Process(std::string name, int linesOfCode) {
//    this->name = name;
//    this->currInstructionLine = 1;
//    this->linesOfCode = linesOfCode;
//    this->timeCreated = std::time(nullptr);
//
//    std::ofstream outfile(this->name);
//
//    outfile << "Process name: " << name << std::endl;
//    outfile << "Logs:" << std::endl << std::endl;
//}
//
//void Process::printCommand(std::ofstream& outfile) {
//    outfile << "(" << this->getTimestamp() << ") " << name << " " << "Core: " << this->cpuCoreID << " " << "Hello World" << std::endl;
//    this->nextLine();
//}
//
// PRINT command
///*
//    Creates a text file where all its
//    associated print commands are written,
//    with the timestamp of when it was executed
//    by the CPU and the CPU core that executed it.
//*/
//void Process::printToTextFile() {
//    std::ofstream outfile(this->name, std::ios::app);
//
//     Write the formatted log entry to the file
//    outfile << "(" << this->getCurrentTimestamp() << ") "
//        << name << " "
//        << "Core: " << this->cpuCoreID << " "
//        << "Hello World" << std::endl;
//
//     Advance the current instruction line
//    this->nextLine();
//}
//
//void Process::draw() const {
//
//    system("cls");
//
//    std::cout << "Process name: " << this->getName() << std::endl;
//    std::cout << "Current line of instruction / Total line of instruction: " << this->getCurrInstructionLine() << " / " << this->getLinesOfCode() << std::endl;
//    std::cout << "Timestamp: " << this->getTimestamp() << std::endl;
//
//    std::string input;
//
//    while (input != "exit") {
//         Get the input string from the user
//        std::cout << "Enter command: ";
//        std::getline(std::cin, input);
//
//         Create a stringstream to split the input
//        std::stringstream ss(input);
//        std::string word;
//        std::vector<std::string> command;
//
//         Split the input string by spaces and store the words in the array
//        while (ss >> word) {
//            command.push_back(word);
//        }
//
//        if (command[0] != "exit") {
//            std::cout << "Invalid command." << std::endl;
//        }
//    }
//}
//
//void Process::nextLine() {
//    this->currInstructionLine += 1;
//}
//
//std::string Process::getName() const {
//    return this->name;
//}
//
//Process::ProcessState Process::getState() const {
//    return this->currentState;
//}
//
//bool Process::isFinished() const {
//    if (currentState == ProcessState::TERMINATED) {
//        return true;
//    }
//
//    return false;
//}
//
//void Process::terminate() {
//    this->currentState = ProcessState::TERMINATED;
//}
//
//int Process::getCurrInstructionLine() const {
//    return this->currInstructionLine;
//}
//
//int Process::getLinesOfCode() const {
//    return this->linesOfCode;
//}
//
//int Process::getCPUCoreID() const {
//    return this->cpuCoreID;
//}
//
//void Process::setCPUCoreID(int cpuCoreID) {
//    this->cpuCoreID = cpuCoreID;
//}
//
//
//std::string Process::getTimestamp() const {
//    std::tm timeInfo;
//    localtime_s(&timeInfo, &timeCreated);
//    std::ostringstream oss;
//    oss << std::put_time(&timeInfo, "(%m/%d/%Y, %I:%M:%S %p)");
//
//    return oss.str();
//}
//
//std::string Process::getCurrentTimestamp() const {
//    std::time_t currentTime = std::time(nullptr);
//    std::tm timeInfo;
//    localtime_s(&timeInfo, &currentTime);
//    std::ostringstream oss;
//    oss << std::put_time(&timeInfo, "(%m/%d/%Y, %I:%M:%S %p)");
//
//    return oss.str();
//}
//
//
//void Process::setTimeCreated() {
//    this->timeCreated = this->getTimestamp();
//}
//
