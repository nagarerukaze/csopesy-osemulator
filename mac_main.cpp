#include <string>
#include "ProcessManager.h"
#include "CPUScheduler.h"
using namespace std;

int main () {
    ProcessManager::getInstance()->initialize(1, 1, 100);
    CPUScheduler::getInstance()->initialize("fcfs", 4, 1000, 500);
    CPUScheduler::getInstance()->initializeCPUWorkers(4);
    
    // Start the scheduler in a detached thread
    std::thread schedulerThread([] {
       CPUScheduler::getInstance()->startScheduler();
    });
    schedulerThread.detach(); // Detach the thread

    std::string input;

    system("clear");

    while (input != "exit") {
       
       // Get the input string from the user
       std::cout << "Enter command: ";
       std::getline(std::cin, input);

        //Create a stringstream to split the input
       std::stringstream ss(input);
       std::string word;
       std::vector<std::string> command;

        //Split the input string by spaces and store the words in the array
       while (ss >> word) {
           command.push_back(word);
       }

       if ((1 < command.size()) && command[0] == "screen" && command[1] == "-r") {
           if ((2 < command.size()) && command[2] != "") {
               int isFound = ProcessManager::getInstance()->displayProcess(command[2]);

               if(isFound) {
                   system("clear");
               }
           }
           else {
               std::cout << "Invalid arguments." << endl;
           }

       }
       else if ((1 < command.size()) && command[0] == "screen" && command[1] == "-s") {
           if ((2 < command.size()) && command[2] != "") {
               ProcessManager::getInstance()->createProcess(command[2]);
               
               int isFound = ProcessManager::getInstance()->displayProcess(command[2]);

               if (isFound) {
                   system("clear");
               }
           }
           else {
               std::cout << "Invalid arguments." << endl;
           }
       }
       else if (command[0] == "screen" && command[1] == "-ls") {
           ProcessManager::getInstance()->displayAllProcesses();
       }
       else {
           std::cout << "Invalid command." << endl;
       }
    }
    
    return 0;
}

