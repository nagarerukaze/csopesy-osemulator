#include "ConsoleManager.h"

// Singleton stuff
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

void ConsoleManager::printHeader() {
    std::cout << " _______  _______  _______  _______  _______  _______  __   __ " << std::endl;
    std::cout << "|       ||       ||       ||       ||       ||       ||  | |  |" << std::endl;
    std::cout << "|       ||  _____||   _   ||    _  ||    ___||  _____||  |_|  |" << std::endl;
    std::cout << "|       || |_____ |  | |  ||   |_| ||   |___ | |_____ |       |" << std::endl;
    std::cout << "|      _||_____  ||  |_|  ||    ___||    ___||_____  ||_     _|" << std::endl;
    std::cout << "|     |_  _____| ||       ||   |    |   |___  _____| |  |   |  " << std::endl;
    std::cout << "|_______||_______||_______||___|    |_______||_______|  |___|  " << std::endl;
    std::cout << "---------------------------------------------------------------" << std::endl;
    std::cout << "Hello, Welcome to CSOPESY Emulator!" << std::endl << std::endl;
    std::cout << "Developers:" << std::endl;
    std::cout << "Jardenil, Aaron Randall (S11)" << std::endl;
    std::cout << "Jocson, Nicole Pedernal (S11)" << std::endl;
    std::cout << "Rebano, Jaeme Patrice (S11)" << std::endl;
    std::cout << "Jaramillo, Brandon Anthony (S12)" << std::endl << std::endl;
    std::cout << "Last updated: 11-03-2024" << std::endl; // TODO: Update the Date
    std::cout << "---------------------------------------------------------------" << std::endl;
    std::cout << std::endl;
}

void ConsoleManager::clear() {
    // For Windows
    system("cls");

    // For Mac
    system("clear");

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
    int num_cpu;
    long long quantum_cycles, batch_process_freq, min_ins, max_ins, delays_per_exec;

    // Get values from config.txt
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

    num_cpu = std::stoi(values[0]);
    scheduler = values[1];
    quantum_cycles = std::stoll(values[2]);
    batch_process_freq = std::stoll(values[3]); // frequency of generating process in "scheduler-test"
    min_ins = std::stoll(values[4]); // min instructions per process 
    max_ins = std::stoll(values[5]); // max instructions per process
    delays_per_exec = std::stoll(values[6]); // delay before executing next instruction

    // Value validation
    if (num_cpu < 1 || num_cpu > 128 ||
        (scheduler != "fcfs" && scheduler != "rr") ||
        quantum_cycles < 1 || quantum_cycles > 4294967296 ||
        batch_process_freq < 1 || batch_process_freq > 4294967296 ||
        min_ins < 1 || min_ins > max_ins || min_ins > 4294967296 ||
        max_ins < 1 || max_ins > 4294967296 ||
        delays_per_exec < 0 || delays_per_exec > 4294967296) {
        return false;
    }

    // Initialize ProcessManager and CPUScheduler
    ProcessManager::getInstance()->initialize(batch_process_freq, min_ins, max_ins);
    CPUScheduler::getInstance()->initialize(scheduler, num_cpu , quantum_cycles, delays_per_exec);
    
    //Start Detached Scheduler Thread
    std::thread schedulerThread([] {
       CPUScheduler::getInstance()->startScheduler();
    });

    schedulerThread.detach(); // Detach the thread

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
            bool isFound = ProcessManager::getInstance()->findProcess(words[2]);
            if (isFound) {
                std::cout << "A process called \"" << words[2] << "\" already exists. Please choose another name." << std::endl;
            }
            else {
                ProcessManager::getInstance()->createProcess(words[2]);
                bool isFound = ProcessManager::getInstance()->displayProcess(words[2]);


                if (isFound) {
                    this->clear();
                }
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

void ConsoleManager::generateProcesses() {
    this->cpuCycles = 1;
    long long count = 1;
    
    while (this->getIsRunning() && ProcessManager::getInstance()->getIsGeneratingProcesses()) {

        if (this->cpuCycles % ProcessManager::getInstance()->getBatchProcessFreq() == 0) {
            String name = (count < 10) ? "p0" + std::to_string(count) : "p" + std::to_string(count);
            ProcessManager::getInstance()->createProcess(name);
            count++;
            this->cpuCycles = 0;
        }

        this->cpuCycles++;
    }
}


void ConsoleManager::schedulerTest() {
    std::cout << "Generating dummy processes..." << std::endl;
    if (ProcessManager::getInstance()->getIsGeneratingProcesses()) {
        std::cout << "Scheduler Test is already running." << std::endl;
        return;
    }

    ProcessManager::getInstance()->setIsGeneratingProcesses(true);
    schedulerTestThread = std::thread(&ConsoleManager::generateProcesses, this);
}

void ConsoleManager::schedulerStop() {
    ProcessManager::getInstance()->setIsGeneratingProcesses(false);

    if (this->schedulerTestThread.joinable()) {
        this->schedulerTestThread.join(); // Wait for the thread to finish
    }
    std::cout << "Stopped generating dummy processes." << std::endl;
}

/*
    Generate a utilization report same as
    screen -ls but is saved into a text
    file - "csopesy-log.txt"
*/
void ConsoleManager::reportUtil() {
    std::ofstream myfile("csopesy-log.txt");
    if (myfile.is_open())
    {
        int coresUsed = CPUScheduler::getInstance()->getNumberOfCPUsUsed();
        int totalCores = CPUScheduler::getInstance()->getNumberOfCores();
        double cpuUtilization = (coresUsed / totalCores) * 100;

        myfile << "CPU Utilization: " << (int)cpuUtilization << "%\n";

        myfile << "Cores used: " << coresUsed << "\n";
        myfile << "Cores available: " << totalCores - coresUsed << "\n\n";
        myfile << "--------------------------------------\n";
        myfile << "Running processes:\n";
        ProcessManager::getInstance()->printActiveProcessesList(myfile);

        myfile << "\n";

        myfile << "Finished processes:\n";
        ProcessManager::getInstance()->printFinishedProcessesList(myfile);
        myfile << "--------------------------------------\n";
        myfile.close();
    }
    else {
        std::cout << "Unable to open file";
    }
}

bool ConsoleManager::getIsRunning() const {
    return this->isRunning;
}

long long ConsoleManager::getCPUCycle() const {
    return this->cpuCycles;
}

void ConsoleManager::stopRunning() {
    this->isRunning = false;

    if (this->cpuThread.joinable()) {
        this->cpuThread.join(); // Wait for the thread to finish
    }
}