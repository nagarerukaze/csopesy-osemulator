#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdlib.h> 
#include <cstdlib>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

#include "ProcessManager.h"
#include "CPUScheduler.h"
#include "Process.h"

void printHeader() {
    cout << " _______  _______  _______  _______  _______  _______  __   __ " << endl;
    cout << "|       ||       ||       ||       ||       ||       ||  | |  |" << endl;
    cout << "|       ||  _____||   _   ||    _  ||    ___||  _____||  |_|  |" << endl;
    cout << "|       || |_____ |  | |  ||   |_| ||   |___ | |_____ |       |" << endl;
    cout << "|      _||_____  ||  |_|  ||    ___||    ___||_____  ||_     _|" << endl;
    cout << "|     |_  _____| ||       ||   |    |   |___  _____| |  |   |  " << endl;
    cout << "|_______||_______||_______||___|    |_______||_______|  |___|  " << endl;
    cout << "---------------------------------------------------------------" << endl;
    cout << "Hello, Welcome to CSOPESY Emulartor!" << endl;
    cout << endl;
    cout << "Developers:" << endl;
    cout << "Jardenil, Aaron Randall (S11)" << endl;
    cout << "Jocson, Nicole Pedernal (S11)" << endl;
    cout << "Rebano, Jaeme Patrice (S11)" << endl;
    cout << "Jaramillo, Brandon Anthony(S12)" << endl;
    cout << endl;
    // TODO: Update the Date
    cout << "Last updated: 10-20-2024" << endl;
    cout << "---------------------------------------------------------------" << endl;
    cout << endl;
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
    system("cls"); // !! CHANGE TO "cls" FOR WINDOWS !!
    printHeader();
}

int main() {
    ProcessManager::getInstance()->initialize();
    CPUScheduler::getInstance()->initialize();
    CPUScheduler::getInstance()->initializeCPUWorkers(4);
    
    // Start the scheduler in a detached thread
    std::thread schedulerThread([] {
        CPUScheduler::getInstance()->startScheduler();
    });
    schedulerThread.detach(); // Detach the thread

    std::string input;

    clear();

    ProcessManager::getInstance()->createProcess("p1", 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ProcessManager::getInstance()->createProcess("p2", 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ProcessManager::getInstance()->createProcess("p3", 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ProcessManager::getInstance()->createProcess("p4", 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ProcessManager::getInstance()->createProcess("p5", 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));




    ProcessManager::getInstance()->createProcess("p6", 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ProcessManager::getInstance()->createProcess("p7", 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ProcessManager::getInstance()->createProcess("p8", 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ProcessManager::getInstance()->createProcess("p9", 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ProcessManager::getInstance()->createProcess("p10", 100);

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

        if ((1 < command.size()) && command[0] == "screen" && command[1] == "-r") {
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
        else if ((1 < command.size()) && command[0] == "screen" && command[1] == "-s") {
            if ((2 < command.size()) && command[2] != "") {
                ProcessManager::getInstance()->createProcess(command[2], 100);
                
                int isFound = ProcessManager::getInstance()->displayProcess(command[2]);

                if (isFound) {
                    clear();
                }
            }
            else {
                std::cout << "Invalid arguments." << endl;
            }
        }
        else if (command[0] == "screen" && command[1] == "-ls") {
            ProcessManager::getInstance()->displayAll();
        }
        else {
            std::cout << "Invalid command." << endl;
        }
    }

}