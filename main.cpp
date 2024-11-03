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
            else if (command == "initialize") {
                std::cout << "Already initialized." << std::endl;
            }
            else {
                std::cout << "Unknown command." << std::endl;
            }
        }
        else if (command == "initialize") {
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
}
