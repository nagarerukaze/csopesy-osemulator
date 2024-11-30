#include "ProcessManager.h"
#include "CPUScheduler.h"

ProcessManager::ProcessManager() {}

// Singleton stuff
ProcessManager::ProcessManager(long long batch_process_freq, long long min_ins, long long max_ins, size_t min_mem_per_proc, size_t max_mem_per_proc, size_t mem_per_frame) {
    this->batch_process_freq = batch_process_freq;
    this->min_ins = min_ins;
    this->max_ins = max_ins;
    this->isGeneratingProcesses = false;
    this->min_mem_per_proc = min_mem_per_proc;
    this->max_mem_per_proc = max_mem_per_proc;
    this->mem_per_frame = mem_per_frame;
    this->processesList.clear();
    // this->finishedProcesses.clear();
}
ProcessManager::ProcessManager(const ProcessManager&) {}

ProcessManager* ProcessManager::sharedInstance = nullptr;

void ProcessManager::initialize(long long batch_process_freq, long long min_ins, long long max_ins, size_t min_mem_per_proc, size_t max_mem_per_proc, size_t mem_per_frame) {
    sharedInstance = new ProcessManager(batch_process_freq, min_ins, max_ins, min_mem_per_proc, max_mem_per_proc, mem_per_frame);
}

ProcessManager* ProcessManager::getInstance() {
    if (sharedInstance == NULL)
    {
        sharedInstance = new ProcessManager;
    }

    return sharedInstance;
}
void ProcessManager::createProcess(const String& name) {
    // std::lock_guard<std::mutex> lock(mtx);
    auto new_process = std::make_shared<Process>(name,
        rand() % (this->max_ins - this->min_ins + 1) + this->min_ins,
        rand() % (this->max_mem_per_proc - this->min_mem_per_proc + 1) + this->min_mem_per_proc, this->mem_per_frame);

    processesList.push_back(new_process);
    CPUScheduler::getInstance()->enqueueProcess(new_process);
}


std::shared_ptr<Process> ProcessManager::findProcess(const String& name) const {
    auto it = std::find_if(processesList.begin(), processesList.end(),
        [&name](const std::shared_ptr<Process>& process) {
            return process->getName() == name && process->getState() != Process::ProcessState::TERMINATED;
        });
    return (it != processesList.end()) ? *it : nullptr;
}

bool ProcessManager::displayProcess(const String& name) const {
    std::shared_ptr<Process> process = findProcess(name);

    // If Found: draw console
    if (process != nullptr) {
        process->draw();
        return true;
    }

    std::cout << "Process '" << name << "' not found." << std::endl;
    return false;
}

/*
    Used in `screen -ls` command.

    Display the activeProcesses (i.e., processes inside a CPU core).
*/
void ProcessManager::displayActiveProcessesList(std::unordered_map<int, std::shared_ptr<Process>> runningProcesses) {
    if (!runningProcesses.empty()) {
        std::vector<int> keys;
        for (const auto& pair : runningProcesses) {
            keys.push_back(pair.first);
        }

        std::sort(keys.begin(), keys.end());

        // Step 3: Print the elements in the order of the sorted keys
        for (int key : keys) {
            std::shared_ptr<Process> processPtr = runningProcesses[key];  // Access process by key
            std::cout << processPtr->getName() << "\t"
                << "(" << processPtr->getTimestamp() << ") \t"
                << "Core: " << key << "\t"
                << processPtr->getCurrentInstructionLine() << "/" << processPtr->getTotalLinesOfCode() << std::endl;
        }
    }

}

/*
    Used in `screen -ls` command.

    Display the processes inside the finishedProcesses list.
*/
void ProcessManager::displayFinishedProcessesList() {
    if (!this->processesList.empty()) {
        auto processesCopy = processesList;

        for (const auto& process : processesCopy) {
            if (process != nullptr && process->getState() == Process::ProcessState::TERMINATED) {
                std::cout << process->getName() << "\t"
                    << "(" << process->getTimestamp() << ") \t"
                    << "Finished!\t"
                    << process->getCurrentInstructionLine() << "/"
                    << process->getTotalLinesOfCode() << std::endl;
            }
        }
    }
}


/*
    Used in `report-util` command.

    Print the activeProcesses (i.e., processes inside a CPU core).
*/
void ProcessManager::printActiveProcessesList(std::ofstream& outFile) {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<CPUWorker*> workers = CPUScheduler::getInstance()->getCPUWorkers();

    for (const auto& worker : workers) {
        if (worker->getProcess() != nullptr) {
            std::shared_ptr<Process> process = worker->getProcess();

            if (process->getState() == Process::ProcessState::RUNNING) {
                outFile<< process->getName() << "\t"
                    << "(" << process->getTimestamp() << ") \t"
                    << "Core: " << std::to_string(process->getCPUCoreID()) << "\t"
                    << process->getCurrentInstructionLine() << "/" << process->getTotalLinesOfCode() << "\n";
            }
        }
    }
}

/*
    Used in `report-util` command.

    Print the processes inside the finishedProcesses list.
*/
void ProcessManager::printFinishedProcessesList(std::ofstream& outFile) {
    std::lock_guard<std::mutex> lock(mtx);

    if (!this->processesList.empty()) {
        auto processesCopy = processesList;

        for (const auto& process : processesCopy) {
            if (process != nullptr && process->getState() == Process::ProcessState::TERMINATED) {
                outFile << process->getName() << "\t"
                    << "(" << process->getTimestamp() << ") \t"
                    << "Finished!\t"
                    << process->getCurrentInstructionLine() << "/"
                    << process->getTotalLinesOfCode() << "\n";
            }
        }
    }
}

/*
    Used in `screen -ls` command.

    Display the following information:
        (1) CPU Utilization %
        (2) # of cores used
        (3) # of cores available
        (4) List of active processes
        (5) List of finished processes

        For each process, display its:
            (1) Name
            (2) Time created
            (3) Core
            (4) Current line of instruction / total lines of code
*/
void ProcessManager::displayAllProcesses() {
    std::lock_guard<std::mutex> lock(mtx);
    int coresUsed = CPUScheduler::getInstance()->getNumberOfCPUsUsed();
    int totalCores = CPUScheduler::getInstance()->getNumberOfCores();
    std::unordered_map<int, std::shared_ptr<Process>> runningProcesses = CPUScheduler::getInstance()->getRunningProcesses();
    
    double cpuUtilization = ((double) coresUsed / (double) totalCores) * 100;

    std::cout << "CPU Utilization: " << cpuUtilization << "%" << std::endl;

    std::cout << "Cores used: " << coresUsed << std::endl;
    std::cout << "Total Cores: " << totalCores << std::endl;
    std::cout << "Cores available: " << totalCores - coresUsed << std::endl;
    std::cout << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Running processes:" << std::endl;
    displayActiveProcessesList(runningProcesses);

    std::cout << std::endl;

    std::cout << "Finished processes:" << std::endl;
    displayFinishedProcessesList();
    std::cout << "--------------------------------------" << std::endl;
}

void ProcessManager::stop() {
    /*this->activeProcesses.clear();
    this->finishedProcesses.clear();*/
    this->processesList.clear();
}

long long ProcessManager::getBatchProcessFreq() const {
    return this->batch_process_freq;
}

long long ProcessManager::getMinInstructions() const {
    return this->min_ins;
}

long long ProcessManager::getMaxInstructions() const {
    return this->max_ins;
}

bool ProcessManager::getIsGeneratingProcesses() const {
    return this->isGeneratingProcesses;
}

void ProcessManager::setIsGeneratingProcesses(bool val) {
    this->isGeneratingProcesses = val;
}

