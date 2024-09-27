#include <iostream>
#include <stdlib.h> 
#include <cstdlib>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

#include "ProcessManager.h"
#include "ProcessManager.cpp"

void printHeader() {
    // put ascii art here
    cout << "  ____ ____   ___  ____  _____ ______ __   __" << endl;
    cout << " / ___/ ___| / _ \\|  _ \\| ____/ ___| \\ \\ / /" << endl;
    cout << "| |   \\___ \\| | | | |_) |  _| \\___/   \\ V / " << endl;
    cout << "| |___ ___) | |_| |  __/| |___ ___) | |   |    " << endl;
    cout << " \\____|____/ \\___/|_|   |_____|____/  |___|  " << endl;
    cout << "Hello, Welcome to CSOPESY command-line!" << endl;

    //cout << "Type 'exit', to quit, 'clear' to clear the screen." << endl;
}

void initialize() {
    // initialize command code here
    cout << "\"initialize\" command recognized. Doing something..." << endl;
}

void screen() {
    // screen command code here
    cout << "\"screen\" command recognized. Doing something..." << endl;
}

void schedulerTest() {
    // scheduler-test command code here
    cout << "\"scheduler-test\" command recognized. Doing something..." << endl;
}

void schedulerStop() {
    // scheduler-stop command code here
    cout << "\"scheduler-stop\" command recognized. Doing something..." << endl;
}

void reportUtil() {
    // report-util command code here
    cout << "\"report-util\" command recognized. Doing something..." << endl;
}

void clear() {
    // clear command code here
    system("clear"); // !! CHANGE TO "cls" FOR WINDOWS !!
    printHeader();
}

int main() {
    ProcessManager::initialize();

    std::string input;

    clear();

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

        if ((1 < command.size()) && command[1] == "-r") {
            if ((2 < command.size()) && command[2] != "") {
                int isFound = ProcessManager::getInstance()->displayProcess(command[2]);

                if(isFound) {
                    clear();
                }
            }
            else {
                std::cout << "Invalid arguments." << endl;
            }

        }
        else if ((1 < command.size()) && command[1] == "-s") {
            if ((2 < command.size()) && command[2] != "") {
                ProcessManager::getInstance()->createProcess(command[2], 100);
            }
            else {
                std::cout << "Invalid arguments." << endl;
            }
        }
        else {
            std::cout << "Invalid command." << endl;
        }
    }

}