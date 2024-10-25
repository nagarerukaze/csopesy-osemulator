#include <string>
#include "ProcessManager.h"
#include "CPUScheduler.h"
using namespace std;

int main () {
    ProcessManager::getInstance()->initialize(1, 1, 100);
    CPUScheduler::getInstance()->initialize("rr", 4, 3000, 500);
    CPUScheduler::getInstance()->initializeCPUWorkers(4);
    
    // !!FOR TESTING, REMOVE THIS WHEN NOT NEEDED ANYMORE!!
    //
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ProcessManager::getInstance()->createProcess("P1");
    ProcessManager::getInstance()->createProcess("P2");
    ProcessManager::getInstance()->createProcess("P3");
    ProcessManager::getInstance()->createProcess("P4");
    ProcessManager::getInstance()->createProcess("P5");
    ProcessManager::getInstance()->createProcess("P6");
    ProcessManager::getInstance()->createProcess("P7");
    ProcessManager::getInstance()->createProcess("P8");
    ProcessManager::getInstance()->createProcess("P9");
    ProcessManager::getInstance()->createProcess("P10");
    ProcessManager::getInstance()->createProcess("P11");
    ProcessManager::getInstance()->createProcess("P12");
    ProcessManager::getInstance()->createProcess("P13");
    ProcessManager::getInstance()->createProcess("P14");
    ProcessManager::getInstance()->createProcess("P15");
    ProcessManager::getInstance()->createProcess("P16");
    ProcessManager::getInstance()->createProcess("P17");
    ProcessManager::getInstance()->createProcess("P18");
    ProcessManager::getInstance()->createProcess("P19");
    ProcessManager::getInstance()->createProcess("P20");
    std::cout << endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    //

    // Start the scheduler in a detached thread

    std::thread schedulerThread([] {
       CPUScheduler::getInstance()->startScheduler();
    });
    schedulerThread.detach(); // Detach the thread

    

    std::string input;

    system("clear");

    while (input != "exit") {
       
       // Get the input string from the user
       std::cout << "Enter command: " << std::endl;
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

