#include "ConsoleManager.h"

typedef std::string String;

int main() {
    String command;
    bool initialized = false;

    ConsoleManager::getInstance()->initializeConsole();
    ConsoleManager::getInstance()->clear();

    while (ConsoleManager::getInstance()->getIsRunning()) { // While OS is running
        // Get Command Input
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);

        if (initialized) {
            if (command.rfind("screen", 0) == 0) {
                ConsoleManager::getInstance()->screen(command);
            }
            else if (command == "scheduler-test") {
                ConsoleManager::getInstance()->schedulerTest();
            }
            else if (command == "scheduler-stop") {
                ConsoleManager::getInstance()->schedulerStop();
            }
            else if (command == "report-util") {
                ConsoleManager::getInstance()->reportUtil();
            }
            else if (command == "clear") {
                ConsoleManager::getInstance()->clear();
            }
            else if (command == "exit") {
                ConsoleManager::getInstance()->stopRunning();
            }
            else if (command == "cycle") {  //TODO: Delete !!
                std::cout << ConsoleManager::getInstance()->getCPUCycle() << std::endl;
            }
            else {
                std::cout << "Unknown command." << std::endl;
            }
        } else if (command == "initialize") {
            initialized = ConsoleManager::getInstance()->initialize();

            if (!initialized) {
                std::cout << "Initialization failed. There are invalid parameters in config.txt." << std::endl;
            }
            else {
                std::cout << "Succesfully initialized." << std::endl;
                ConsoleManager::getInstance()->startCPUCycle();
            }
        }
        else if (command == "clear") {
            ConsoleManager::getInstance()->clear();
        }
        else if (command == "exit") {
            ConsoleManager::getInstance()->stopRunning();
        }
        else {
            std::cout << "Unknown command." << std::endl;
        }
    }

    return 0;

    //ProcessManager::getInstance()->initialize();
    //CPUScheduler::getInstance()->initialize();
    //CPUScheduler::getInstance()->initializeCPUWorkers(4);
    //
    // Start the scheduler in a detached thread
    //std::thread schedulerThread([] {
    //    CPUScheduler::getInstance()->startScheduler();
    //});
    //schedulerThread.detach(); // Detach the thread

    //std::string input;

    //clear();

    //while (input != "exit") {
    //    
    //     Get the input string from the user
    //    std::cout << "Enter command: ";
    //    std::getline(std::cin, input);

    //     Create a stringstream to split the input
    //    std::stringstream ss(input);
    //    std::string word;
    //    std::vector<std::string> command;

    //     Split the input string by spaces and store the words in the array
    //    while (ss >> word) {
    //        command.push_back(word);
    //    }

    //    if ((1 < command.size()) && command[0] == "screen" && command[1] == "-r") {
    //        if ((2 < command.size()) && command[2] != "") {
    //            int isFound = ProcessManager::getInstance()->displayProcess(command[2]);

    //            if(isFound) {
    //                clear();
    //            }
    //        }
    //        else {
    //            std::cout << "Invalid arguments." << endl;
    //        }

    //    }
    //    else if ((1 < command.size()) && command[0] == "screen" && command[1] == "-s") {
    //        if ((2 < command.size()) && command[2] != "") {
    //            ProcessManager::getInstance()->createProcess(command[2], 100);
    //            
    //            int isFound = ProcessManager::getInstance()->displayProcess(command[2]);

    //            if (isFound) {
    //                clear();
    //            }
    //        }
    //        else {
    //            std::cout << "Invalid arguments." << endl;
    //        }
    //    }
    //    else if (command[0] == "screen" && command[1] == "-ls") {
    //        ProcessManager::getInstance()->displayAll();
    //    }
    //    else {
    //        std::cout << "Invalid command." << endl;
    //    }
    //}

}

