#include "ConsoleManager.h"

/*

    Singleton stuff

*/
ConsoleManager::ConsoleManager() {}
ConsoleManager::ConsoleManager(const ConsoleManager&) {}

ConsoleManager* ConsoleManager::sharedInstance = nullptr;

void ConsoleManager::initialize() {
    sharedInstance = new ConsoleManager();
}

ConsoleManager* ConsoleManager::getInstance() {
    return sharedInstance;
}

void ConsoleManager::printHeader() {
    std::cout << " _______  _______  _______  _______  _______  _______  __   __ " << std::endl;
    std::cout << "|       ||       ||       ||       ||       ||       ||  | |  |" << std::endl;
    std::cout << "|       ||  _____||   _   ||    _  ||    ___||  _____||  |_|  |" << std::endl;
    std::cout << "|       || |_____ |  | |  ||   |_| ||   |___ | |_____ |       |" << std::endl;
    std::cout << "|      _||_____  ||  |_|  ||    ___||    ___||_____  ||_     _|" << std::endl;
    std::cout << "|     |_  _____| ||       ||   |    |   |___  _____| |  |   |  " << std::endl;
    std::cout << "|_______||_______||_______||___|    |_______||_______|  |___|  " << std::endl;
    std::cout << "---------------------------------------------------------------" << std::endl;
    std::cout << "Hello, Welcome to CSOPESY Emulartor!" << std::endl << std::endl;
    std::cout << "Developers:" << std::endl;
    std::cout << "Jardenil, Aaron Randall (S11)" << std::endl;
    std::cout << "Jocson, Nicole Pedernal (S11)" << std::endl;
    std::cout << "Rebano, Jaeme Patrice (S11)" << std::endl;
    std::cout << "Jaramillo, Brandon Anthony (S12)" << std::endl << std::endl;
    std::cout << "Last updated: 10-20-2024" << std::endl; // TODO: Update the Date
    std::cout << "---------------------------------------------------------------" << std::endl;
    std::cout << std::endl;
}

void ConsoleManager::clear() {
    system("cls"); // !! CHANGE TO "cls" FOR WINDOWS !!
    this->printHeader();
}

/*
    Function for handling `screen` commands:
        (1) screen -ls: Display CPU usage and list of active/finished processes
        (2) screen -s: Create a new process
        (3) screen -r: Access a process
*/
void ConsoleManager::screen(String command) {
    std::cout << "\"screen\" command recognized. Doing something..." << std::endl; // TODO: DELETE

    std::istringstream iss(command);
    std::vector<String> words;
    String word;

    while (iss >> word) {
        words.push_back(word);
    }

    /*
        `screen -ls`

        Display the following:
            (1) CPU utilization
            (2) Cores used
            (3) List of active processes
            (4) List of finished processes
     */
    if (words[1] == "-ls") {
        // TODO: 
        std::cout << "Show running processes." << std::endl;

        if (words.size() == 2) {

        }

        // Invalid command input
        else {
            std::cout << "Invalid arguments." << std::endl;
        }
    }

    /*
        `screen -s`

        Creates a new process
     */
    else if (words[1] == "-s") {

        // In this one, user can type:
        // "process-smi" - prints simple info of the process
            // Process: My own process
            // ID: 1
            // Current instruction line: 769
            // Lines of code: 1240
            // or
            // Finished!
        // "exit : returns to main menu
        // once user exits a process that has finished, they cannot access it again

        if (words.size() == 3) {
            // TODO: Create Process i.e.g add to active processes
            // TODO: Display Process Screen
            std::cout << "Create process." << std::endl;

            /*Process* process = new Process(words[2], 100);
            process->draw();
            std::cout << "Exited." << std::endl;*/
            clear();
        }

        // Invalid command input
        else {
            std::cout << "Invalid arguments." << std::endl;
        }
    }

    /*
        `screen -r <process name>`

        Access a process.

        (1) Redraw console
        (2) Display:
            * name,
            * current instruction line,
            * lines of code
     */
    else if (words[1] == "-r") {
        // if process not found/finished execution : print "Process <process name> not found."

        if (words.size() == 3) {
            // TODO: Find Process
            std::cout << "Find process." << std::endl;
        }

        // Invalid command
        else {
            std::cout << "Invalid arguments." << std::endl;
        }
    }

    // Invalid command
    else {
        std::cout << "Unknown command." << std::endl;
    }
}