#include "ConsoleManager.h"

/*

    Singleton stuff

*/
ConsoleManager::ConsoleManager() {
    this->cpuCycles = 0;
    this->isRunning = true;
}
ConsoleManager::ConsoleManager(const ConsoleManager&) {}

ConsoleManager* ConsoleManager::sharedInstance = nullptr;

void ConsoleManager::initializeConsole() {
    sharedInstance = new ConsoleManager();
}

ConsoleManager* ConsoleManager::getInstance() {
    return sharedInstance;
}

/*
    TODO: Honestly not sure if this is how the cpu cycles work !!
*/
void ConsoleManager::startCPUCycle() {
    this->cpuThread = std::thread([this]() { // Create the CPU cycle thread
        int cpuCycle = 0; // Example CPU cycle counter
        while (this->getIsRunning()) {
            this->cpuCycles++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate CPU cycle
        }
        });
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
    Get the following parameters from the `config.txt` file:
        (1) num-cpu -> scheduler
        (2) scheduler -> scheduler
        (3) quantum-cycles -> in scheduler
        (4) batch-process-freq -> process manager
        (5) min-ins -> process manager
        (6) max-ins -> process manager
        (7) delay-per-exec -> in scheduler
*/
bool ConsoleManager::initialize() {
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
    ProcessManager::getInstance()->initialize(batch_process_freq, min_ins, max_ins);
    // CPUScheduler::getInstance()->initialize(num_cpu, scheduler, quantum_cycles, delay_per_exec);

    return true;
}

/*
    Function for handling `screen` commands:
        (1) screen -ls: Display CPU usage and list of active/finished processes
        (2) screen -s: Create a new process
        (3) screen -r: Access a process
*/
void ConsoleManager::screen(String command) {
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
            ProcessManager::getInstance()->displayAllProcesses();
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
        if (words.size() == 3) {
            ProcessManager::getInstance()->createProcess(words[2]);
            bool isFound = ProcessManager::getInstance()->displayProcess(words[2]);

            if (isFound) {
                this->clear();
            }
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
        if (words.size() == 3) {
            bool isFound = ProcessManager::getInstance()->displayProcess(words[2]);

            if (isFound) {
                this->clear();
            }
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

void ConsoleManager::schedulerTest() {
    std::cout << "Generating processes..." << std::endl; // TODO: DELETE
    if (ProcessManager::getInstance()->getIsGeneratingProcesses()) {
        std::cout << "Scheduler Test is already running." << std::endl;
        return;
    }
    ProcessManager::getInstance()->setIsGeneratingProcesses(true);

    schedulerTestThread = std::thread([this]() {
        ProcessManager::getInstance()->createProcess("p01"); // Create First Process
        int count = 2;
        while (this->getIsRunning() && ProcessManager::getInstance()->getIsGeneratingProcesses()) {
            // Wait for the specified number of CPU cycles
            int currentCycle = this->getCPUCycle();
            int targetCycle = currentCycle + ProcessManager::getInstance()->getBatchProcessFreq();

            // Keep incrementing the CPU cycles until the target is reached
            while (this->getCPUCycle() < targetCycle) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Adjust sleep time as needed
            }
            String name = "";
            if (count < 10) {
                name = "p0" + count;
            }
            else {
                name = "p" + count;
            }
            ProcessManager::getInstance()->createProcess(name);
            std::cout << "Created process at Cycle " + this->getCPUCycle() << std::endl;
            // std::cout << "Created process: " << name << std::endl; // DELETE!! Log the created process
        }
        });
}

void ConsoleManager::schedulerStop() {
    // TODO: stops generating dummy processes
    // only accessible in main menu
    std::cout << "\"scheduler-stop\" command recognized. Doing something..." << std::endl; // TODO: DELETE
    ProcessManager::getInstance()->setIsGeneratingProcesses(false);

    if (this->schedulerTestThread.joinable()) {
        this->schedulerTestThread.join(); // Wait for the thread to finish
    }
}

void ConsoleManager::reportUtil() {
    // TODO: generates CPU utilization report
    // This console should be able to generate a utilization report whenever the "report-util command is entered
    // same as screen -ls but is saved into a text file - "csopesy-log.txt"
    std::cout << "\"report-util\" command recognized. Doing something..." << std::endl; // TODO: DELETE
}

void ConsoleManager::setCursorPosition(int x, int y) {

    COORD coord;
    coord.X = x; // Column
    coord.Y = y; // Row

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void ConsoleManager::printAtPosition(int x, int y, const String& text) {
    setCursorPosition(x, y);
    std::cout << text;
}


bool ConsoleManager::getIsRunning() const {
    return this->isRunning;
}

int ConsoleManager::getCPUCycle() const {
    return this->cpuCycles;
}

void ConsoleManager::stopRunning() {
    this->isRunning = false;

    if (this->cpuThread.joinable()) {
        this->cpuThread.join(); // Wait for the thread to finish
    }
}


