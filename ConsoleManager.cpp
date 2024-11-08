#include "ConsoleManager.h"

// Singleton stuff
ConsoleManager::ConsoleManager() {
    this->isRunning = true;
}

ConsoleManager::ConsoleManager(const ConsoleManager&) {}

ConsoleManager* ConsoleManager::sharedInstance = nullptr;

void ConsoleManager::initializeConsole() {
    sharedInstance = new ConsoleManager();
}

ConsoleManager* ConsoleManager::getInstance() {
    if (sharedInstance == NULL)
    {
        sharedInstance = new ConsoleManager;
    }

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
    std::cout << "Last updated: 11-08-2024" << std::endl; // TODO: Update the Date
    std::cout << "---------------------------------------------------------------" << std::endl;
    std::cout << std::endl;
}

void ConsoleManager::clear() {
    // For Windows
    system("cls");

    // For Mac
    //system("clear");

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
    int num_cpu, max_overall_mem, mem_per_frame, mem_per_proc;
    long long quantum_cycles, batch_process_freq, min_ins, max_ins, delays_per_exec;

    // Get values from config.txt
    std::ifstream f("config.txt");

    if (!f.is_open()) {
        std::cerr << "Error: config.txt could not be opened!" << std::endl;
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

    try {
        num_cpu = std::stoi(values[0]);
        scheduler = values[1];
        quantum_cycles = std::stoll(values[2]);
        batch_process_freq = std::stoll(values[3]);
        min_ins = std::stoll(values[4]);
        max_ins = std::stoll(values[5]);
        delays_per_exec = std::stoll(values[6]);
        max_overall_mem = std::stoi(values[7]);
        mem_per_frame = std::stoi(values[8]);
        mem_per_proc = std::stoi(values[9]);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: Conversion error - " << e.what() << std::endl;
        return false;
    }

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
    ProcessManager::getInstance()->initialize(batch_process_freq, min_ins, max_ins, mem_per_proc);
    CPUScheduler::getInstance()->initialize(scheduler, num_cpu , quantum_cycles, delays_per_exec);
    MemoryManager::getInstance()->initialize(max_overall_mem, mem_per_frame);
    
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
            if (ProcessManager::getInstance()->findProcess(words[2]) != nullptr) {
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
    long long lastCycle = CPUScheduler::cpuCycles;
    long long batchCycleCounter = 1;
    long long count = 1;
    long long leftPart = 0;
    const size_t maxNameLength = 16;

    while (this->getIsRunning() && ProcessManager::getInstance()->getIsGeneratingProcesses()) {
        if (CPUScheduler::cpuCycles != lastCycle) {
            lastCycle = CPUScheduler::cpuCycles;

            if (batchCycleCounter % ProcessManager::getInstance()->getBatchProcessFreq() == 0) {
                String name;

                // Construct the name with leftPart and count
                if (count < 10) {
                    name = std::to_string(leftPart) + "p0" + std::to_string(count);
                }
                else {
                    name = std::to_string(leftPart) + "p" + std::to_string(count);
                }

                // Ensure the process name does not exceed 18 characters
                if (name.length() > maxNameLength) {
                    // If the name exceeds the max length, increment leftPart and reset count
                    leftPart++;
                    count = 1; // Reset count to start fresh with the next batch
                }

                // Create the process with the generated name
                ProcessManager::getInstance()->createProcess(name);
                count++;
                batchCycleCounter = 0;
            }
            batchCycleCounter++;
        }
    }
}



void ConsoleManager::schedulerTest() {
    if (ProcessManager::getInstance()->getIsGeneratingProcesses()) {
        std::cout << "Scheduler Test is already running." << std::endl;
        return;
    }

    ProcessManager::getInstance()->setIsGeneratingProcesses(true);
    std::cout << "Generating dummy processes..." << std::endl;
    schedulerTestThread = std::thread(&ConsoleManager::generateProcesses, this);
}

void ConsoleManager::schedulerStop() {
    if(!ProcessManager::getInstance()->getIsGeneratingProcesses()) {
        std::cout << "Scheduler Test is not currently running." << std::endl;
        return;
    }
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
        std::cout << "Report generated in csopesy-log.txt" << std::endl;
    }
    else {
        std::cout << "Unable to open file. Report was not successfully generated." << std::endl;
    }
}

bool ConsoleManager::getIsRunning() const {
    return this->isRunning;
}

void ConsoleManager::stopRunning() {
    this->isRunning = false;

    if (this->schedulerTestThread.joinable()) {
        this->schedulerTestThread.join(); // Wait for the thread to finish
    }

    CPUScheduler::getInstance()->stopScheduler();
}