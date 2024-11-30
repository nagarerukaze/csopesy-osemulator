#include "ConsoleManager.h"
#include "Process.h" //TODO: Delete

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
    std::cout << "Last updated: 11-30-2024" << std::endl; // TODO: Update the Date
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

bool ConsoleManager::isOutsideRange(long long val, long long min) {
    return (val < min || val > 4294967296);
}

bool ConsoleManager::isOutsideMemoryRange(size_t val) {
    if (val < 2 || val > 4294967296) {
        return true;
    }
    return (val & (val - 1)) != 0;
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
    String line, key, value, scheduler, allocator;
    int num_cpu;
    size_t max_overall_mem, mem_per_frame, min_mem_per_proc, max_mem_per_proc;
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
        mem_per_frame = std::stoull(values[8]);
        min_mem_per_proc = std::stoull(values[9]);
        max_mem_per_proc = std::stoull(values[10]);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: Conversion error - " << e.what() << std::endl;
        return false;
    }

    // Value validation
    if (num_cpu < 1 || num_cpu > 128 ||
        (scheduler != "fcfs" && scheduler != "rr") ||
        isOutsideRange(quantum_cycles, 1) ||
        isOutsideRange(batch_process_freq, 1) ||
        isOutsideRange(min_ins, 1) || min_ins > max_ins ||
        isOutsideRange(max_ins, 1) ||
        isOutsideRange(delays_per_exec, 0)) {
        return false;
    }

    // Memory validation
    if (isOutsideMemoryRange(max_overall_mem) ||
        isOutsideMemoryRange(mem_per_frame) ||
        isOutsideMemoryRange(min_mem_per_proc) ||
        isOutsideMemoryRange(max_mem_per_proc)) {
        return false;
    }

    // Flat Mem Allocator or Paging Allocator?
    if (max_overall_mem == mem_per_frame) {
        allocator = "flat";
    }
    else {
        allocator = "paging";
    }

    this->memPerFrame = mem_per_frame;

    // Initialize ProcessManager and CPUScheduler
    ProcessManager::getInstance()->initialize(batch_process_freq, min_ins, max_ins, min_mem_per_proc, max_mem_per_proc, mem_per_frame);
    CPUScheduler::getInstance()->initialize(scheduler, num_cpu , quantum_cycles, delays_per_exec, allocator);
    MemoryManager::getInstance()->initialize(max_overall_mem);
    PagingAllocator::getInstance()->initialize(max_overall_mem, mem_per_frame);

    std::ofstream outFile("backing_store.txt", std::ios::trunc); // Open in truncate mode
    if (!outFile) {
        std::cerr << "Error: Could not create or clear the file.\n";
    }
    else {
        std::cout << "File cleared: " << "backing_store" << "\n";
    }
    outFile.close();
    
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
        // std::cout << "SCHEDULER CYCLE IN schedyler-test: " << CPUScheduler::cpuCycles << std::endl;
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
    
    schedulerTestThread = std::thread(&ConsoleManager::generateProcesses, this);
    schedulerTestThread.detach();
    std::cout << "Generating dummy processes..." << std::endl;
}

void ConsoleManager::schedulerStop() {
    if(!ProcessManager::getInstance()->getIsGeneratingProcesses()) {
        std::cout << "Scheduler Test is not currently running." << std::endl;
        return;
    }
    ProcessManager::getInstance()->setIsGeneratingProcesses(false);

    if (this->schedulerTestThread.joinable()) {
        std::cout << "Waiting for thread to finish..." << std::endl;
        this->schedulerTestThread.join(); // Wait for the thread to finish
        std::cout << "Thread has finished." << std::endl;
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

/*
    Provides a summarized view of the available/used memory, as well as 
    the list of processes and memory occupied. This is similar to the “nvidia-smi” 
    command. 
*/
void ConsoleManager::processSMI() {

    // CPU utilization
    int coresUsed = 0;
    int totalCores = 1;
    double cpuUtil = 0;

    // Memory usage and utilization
    size_t usedMemory = 0;
    size_t totalMemory = 1;
    size_t memoryUtil = 0;

    coresUsed = CPUScheduler::getInstance()->getNumberOfCPUsUsed();
    totalCores = CPUScheduler::getInstance()->getNumberOfCores();
    cpuUtil = ((double)coresUsed / (double)totalCores) * 100;

    usedMemory = MemoryManager::getInstance()->getAllocatedSize();
    totalMemory = MemoryManager::getInstance()->getMaximumMemory();
    memoryUtil = (usedMemory / totalMemory) * 100;

    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "| PROCESS-SMI V01.00 Driver Version: 01.00 |" << std::endl;
    std::cout << "CPU-Util: " << cpuUtil << "%" << std::endl; // TODO
    std::cout << "Memory Usage: " << usedMemory << "MiB / " << totalMemory << "MiB" << std::endl; // TODO
    std::cout << "Memory Util: " << memoryUtil << "%" << std::endl << std::endl << std::endl; // TODO

    std::cout << "===============================================" << std::endl;
    std::cout << "Running processes and memory usage:" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    MemoryManager::getInstance()->displayRunningProcsAndMemUsage();
    std::cout << "----------------------------------------------" << std::endl;
}

/*
    Total Memory in KB
    Used Memory: Total active memory used by processes
    Free memory: Total free memory that can still be used by other processes
    Idle CPU ticks: number of ticks wherein the CPU cores remained idle
    Active CPU ticks: number of ticks wherein CPU cores are actually executing instructions
    Total CPU ticks: number of ticks that passed for all CPU cores
    Num paged in: Accumulated number of pages paged in
    Num paged out: Accumulated number of pages paged out
*/
void ConsoleManager::vmstat() {

    size_t totalMem = 0;
    size_t usedMem = 0;
    size_t freeMem = 0;
    size_t idleTicks = 0;
    size_t activeTicks = 0;
    size_t totalTicks = 0;
    size_t pagedIn = 0;
    size_t pagedOut = 0;

    if (CPUScheduler::getInstance()->getAllocator() == "flat") {
        totalMem = MemoryManager::getInstance()->getMaximumMemory();
        usedMem = MemoryManager::getInstance()->getAllocatedSize();
        freeMem = totalMem - usedMem;
    }
    else if (CPUScheduler::getInstance()->getAllocator() == "paging") {
        totalMem = this->memPerFrame * PagingAllocator::getInstance()->getNumOfFrames();
        freeMem = this->memPerFrame * PagingAllocator::getInstance()->getFreeFrames();
        usedMem = totalMem - freeMem;
    }

    idleTicks = CPUScheduler::getInstance()->getIdleCPUTicks();
    activeTicks = CPUScheduler::getInstance()->getActiveCPUTicks();
    totalTicks = idleTicks + activeTicks;
    pagedIn = PagingAllocator::getInstance()->getNumPagedIn();
    pagedOut = PagingAllocator::getInstance()->getNumPagedOut();
    std::vector<CPUWorker*> cpuWorkers = CPUScheduler::getInstance()->getCPUWorkers();

    std::cout << "Total Memory in KB: " << totalMem << std::endl;
    std::cout << "Used Memory: " << usedMem << std::endl;
    std::cout << "Free memory: " << freeMem << std::endl;
    std::cout << "Idle CPU ticks: " << idleTicks <<std::endl;
    std::cout << "Active CPU ticks: " << std::endl;
    for (const auto& worker : cpuWorkers) {
        std::cout << "Core " << worker->getID() << ": " << worker->getActiveCPUTicks() << std::endl;
    }
    std::cout << "Total CPU ticks: " << totalTicks << std::endl;
    std::cout << "Num paged in: " << pagedIn << std::endl;
    std::cout << "Num paged out: " << pagedOut << std::endl << std::endl;
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
    ProcessManager::getInstance()->stop();
}


void ConsoleManager::test() {
    // std::queue<String> orderOfProcesses

    //PagingAllocator::getInstance()->initialize(1024, 256);

    //std::cout << "Initial memory: " << std::endl;
    //PagingAllocator::getInstance()->visualizeMemory();

    //std::shared_ptr<Process> p = std::make_shared<Process>("P1", 4000, 512, 256);
    //std::shared_ptr<Process> p2 = std::make_shared<Process>("P2", 4000, 512, 256);
    //std::shared_ptr<Process> p3 = std::make_shared<Process>("P3", 4000, 512, 256);
    //std::shared_ptr<Process> p4 = std::make_shared<Process>("P4", 4000, 512, 256);


    //void* ptr1 = PagingAllocator::getInstance()->allocate(p);
    //std::cout << "Pointer address: " << reinterpret_cast<uintptr_t>(ptr1) << std::endl;
    //if (ptr1 == nullptr) {
    //    std::cout << "it is null" << std::endl;
    //}
    //else {
    //    std::cout << "it is not null" << std::endl;
    //}

    //std::cout << "Memory after allocating p:" << std::endl;
    //PagingAllocator::getInstance()->visualizeMemory();

    //PagingAllocator::getInstance()->deallocate(p);

    //std::cout << "Memory after deallocating p:" << std::endl;
    //PagingAllocator::getInstance()->visualizeMemory();

    //void* ptr2 = PagingAllocator::getInstance()->allocate(p2);
    //std::cout << "Pointer address: " << reinterpret_cast<uintptr_t>(ptr2) << std::endl;

    //std::cout << "Memory after allocating p2:" << std::endl;
    //PagingAllocator::getInstance()->visualizeMemory();

    //void* ptr3 = PagingAllocator::getInstance()->allocate(p);
    //std::cout << "Pointer address: " << reinterpret_cast<uintptr_t>(ptr3) << std::endl;

    //std::cout << "Memory after allocating p:" << std::endl;
    //PagingAllocator::getInstance()->visualizeMemory();

    //void* ptr4 = PagingAllocator::getInstance()->allocate(p3);
    //std::cout << "Pointer address: " << reinterpret_cast<uintptr_t>(ptr4) << std::endl;

    //std::cout << "Memory after allocating p3:" << std::endl;
    //PagingAllocator::getInstance()->visualizeMemory();

    //PagingAllocator::getInstance()->saveProcessToBS(p3);
    //std::cout << "Memory after putting p3 in backing store:" << std::endl;
    //PagingAllocator::getInstance()->visualizeMemory();
    //PagingAllocator::getInstance()->deallocate(p);

    //PagingAllocator::getInstance()->saveProcessToBS(p4);
    //std::cout << "Memory after putting p3 in backing store:" << std::endl;
    //PagingAllocator::getInstance()->visualizeMemory();
    //PagingAllocator::getInstance()->deallocate(p);

    //PagingAllocator::getInstance()->removeProcessFromBS(p4);

    //std::cout << "Memory after deallocating p:" << std::endl;
    //PagingAllocator::getInstance()->visualizeMemory();

    //void* ptr5 = PagingAllocator::getInstance()->allocate(p4);
    //std::cout << "Pointer address: " << reinterpret_cast<uintptr_t>(ptr5) << std::endl;

    //std::cout << "Memory after allocating p4:" << std::endl;
    //PagingAllocator::getInstance()->visualizeMemory();


    /*
    // Initialize the memory with 20 units
    MemoryManager::getInstance()->initialize(16384);

    // Visualize initial memory state
    std::cout << "Initial memory: " << MemoryManager::getInstance()->visualizeMemory() << std::endl;

    // Allocate 5 blocks
    void* ptr1 = MemoryManager::getInstance()->allocate(4096);
    std::cout << "Memory after allocating 5 blocks: " << MemoryManager::getInstance()->visualizeMemory() << std::endl;

    // Allocate 4 more blocks
    void* ptr2 = MemoryManager::getInstance()->allocate(4096);
    std::cout << "Memory after allocating 4 more blocks: " << MemoryManager::getInstance()->visualizeMemory() << std::endl;

    // Allocate 4 more blocks
    void* ptr3 = MemoryManager::getInstance()->allocate(4096);
    std::cout << "Memory after allocating 4 more blocks: " << MemoryManager::getInstance()->visualizeMemory() << std::endl;

    // Allocate 4 more blocks
    void* ptr4 = MemoryManager::getInstance()->allocate(4096);
    std::cout << "Memory after allocating 4 more blocks: " << MemoryManager::getInstance()->visualizeMemory() << std::endl;

    // Allocate 4 more blocks
    void* ptr5 = MemoryManager::getInstance()->allocate(4096);
    std::cout << (ptr5 == nullptr) << std::endl;

    // Deallocate the first allocation
    MemoryManager::getInstance()->deallocate(ptr1, 4096);
    std::cout << "Memory after deallocating first allocation: " << MemoryManager::getInstance()->visualizeMemory() << std::endl;

    ptr5 = MemoryManager::getInstance()->allocate(4096);
    std::cout << (ptr5 == nullptr) << std::endl;
    std::cout << "Memory after allocating 4 more blocks: " << MemoryManager::getInstance()->visualizeMemory() << std::endl;

    */
}
