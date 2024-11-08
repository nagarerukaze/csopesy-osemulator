#include <string>
#include <vector>
#include <mutex>
#include <memory>

#include "Process.h"
#include "CPUWorker.h"

typedef std::string String;

class ProcessManager 
{
public:
    // Singleton stuff
    static void initialize(long long batch_process_freq, long long min_ins, long long max_ins, size_t mem_per_proc);
    static ProcessManager* getInstance();

    void createProcess(const String& name);
    std::shared_ptr<Process> findProcess(const String& name) const;
    bool displayProcess(const String& name) const;
    void displayActiveProcessesList();
    void displayFinishedProcessesList();
    void printActiveProcessesList(std::ofstream& outFile);
    void printFinishedProcessesList(std::ofstream& outFile);
    void displayAllProcesses();

    long long getBatchProcessFreq() const;
    long long getMinInstructions() const;
    long long getMaxInstructions() const;
    bool getIsGeneratingProcesses() const;
    void moveToFinished(std::shared_ptr<Process> process);

    void setIsGeneratingProcesses(bool val);

private:
    ProcessManager();
    ProcessManager(long long batch_process_freq, long long min_ins, long long max_ins, size_t mem_per_proc);
    ProcessManager(const ProcessManager&);
    ProcessManager& operator = (const ProcessManager&);
    ~ProcessManager() = default;
    static ProcessManager* sharedInstance;
    
    long long batch_process_freq;
    long long min_ins;
    long long max_ins;
    bool isGeneratingProcesses;
    size_t mem_per_proc;

    std::vector<std::shared_ptr<Process>> activeProcesses;
    std::vector<std::shared_ptr<Process>> finishedProcesses;
    std::mutex mtx;
};