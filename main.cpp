#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h> 
#include <conio.h>
#include <string>
#include <vector>

#include "ConsoleManager.h"
#include "Process.h"

typedef std::string String;

/*
    Get the following parameters from the `config.txt` file:
        (1) num-cpu
        (2) scheduler
        (3) quantum-cycles
        (4) batch-process-freq
        (5) min-ins
        (6) max-ins
        (7) delay-pe-exec
*/
bool initialize() {
    std::cout << "\"initialize\" command recognized. Doing something..." << std::endl; // TODO: DELETE
    std::vector<String> values;
    String line, key, value, scheduler;
    int num_cpu, quantum_cycles, batch_process_freq, min_ins, max_ins, delays_per_exec;

    std::ifstream f("config.txt");

    if (!f.is_open()) {
        std::cerr << "Error opening the file!";
        return false;
    }

    while (getline(f, line)) {
        std::istringstream iss(line);
        iss >> key;
        getline(iss >> std::ws, value);

        if (value.front() == '\"' && value.back() == '\"') {
            value = value.substr(1, value.length() - 2);
        }
        values.push_back(value);
    }

    f.close();

    num_cpu = stoi(values[0]);
    scheduler = values[1];
    quantum_cycles = stoi(values[2]);
    batch_process_freq = stoi(values[3]); // frequency of generating process in "scheduler-test". [1, 2^32] if one, a new process is generated at the end of each CPU cycle
    min_ins = stoi(values[4]); // min instructions per process [1, 2^32]
    max_ins = stoi(values[5]); // max instructions per process [1, 2^32]
    delays_per_exec = stoi(values[6]); // delay before executing next instruction. [0, 2^32] if zero, each instruction is executed per CPU cycle

    if (num_cpu < 1 || num_cpu > 128 ||
        (scheduler != "fcfs" && scheduler != "rr") ||
        quantum_cycles < 1 || quantum_cycles > 4294967296 ||
        batch_process_freq < 1 || batch_process_freq > 4294967296 ||
        min_ins < 1 || min_ins > max_ins || min_ins > 4294967296 ||
        max_ins < 1 || max_ins > 4294967296 ||
        delays_per_exec < 0 || delays_per_exec > 4294967296) {
        return false;
    }

    // TODO: initilize processor and scheduler algo

    return true;
}

void schedulerTest() {
    // TODO: continuosly generates a batch of dummy processes for the CPU scheduler
    // Every X CPU cycles, a new process is generated and put into ready queue (Can be set in "config.txt"
    // As long as CPU cores are available, each process can be executed and accessible via "screen"
    // only accessible in main menu
    // process names should be "p01, p02, ..., p1240"
    std::cout << "\"scheduler-test\" command recognized. Doing something..." << std::endl; // TODO: DELETE
}

void schedulerStop() {
    // TODO: stops generating dummy processes
    // only accessible in main menu
    std::cout << "\"scheduler-stop\" command recognized. Doing something..." << std::endl; // TODO: DELETE
}

void reportUtil() {
    // TODO: generates CPU utilization report
    // This console should be able to generate a utilization report whenever the "report-util command is entered
    // same as screen -ls but is saved into a text file - "csopesy-log.txt"
    std::cout << "\"report-util\" command recognized. Doing something..." << std::endl; // TODO: DELETE
}

int main() {
    int cpuCycles = 0;
    String command;
    bool initialized = false;
    bool isRunning = true;

    ConsoleManager::getInstance()->clear();

    while (isRunning) { // While OS is running
        // Get Command Input
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);

        if (command == "initialize") {
            initialized = initialize();

            if (!initialized) {
                std::cout << "There are invalid parameters in config.txt." << std::endl;
            }
        }
        else if (initialized) {
            if (command.rfind("screen", 0) == 0) {
                ConsoleManager::getInstance()->screen(command);
            }
            else if (command == "scheduler-test") {
                schedulerTest();
            }
            else if (command == "scheduler-stop") {
                schedulerStop();
            }
            else if (command == "report-util") {
                reportUtil();
            }
            else if (command == "clear") {
                ConsoleManager::getInstance()->clear();
            }
            else if (command == "exit") {
                isRunning = false;
            }
            else {
                std::cout << "Unknown command." << std::endl;
            }
        }
        else if (command == "clear") {
            ConsoleManager::getInstance()->clear();
        }
        else if (command == "exit") {
            isRunning = false;
        }
        else {
            std::cout << "Unknown command." << std::endl;
        }

        cpuCycles++;
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

