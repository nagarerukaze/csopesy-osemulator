#include <string>
#include <vector>

#include "Process.h"

typedef std::string String;

class ProcessManager 
{
public:
    // Singleton stuff
    static void initialize(long long batch_process_freq, long long min_ins, long long max_ins);
    static ProcessManager* getInstance();

    void createProcess(String name);
    Process* findProcess(const String& name) const;
    bool displayProcess(const String& name) const;
    void displayProcessesList(std::vector<Process*> processList);
    void displayAllProcesses();

    long long getBatchProcessFreq() const;
    long long getMinInstructions() const;
    long long getMaxInstructions() const;
    bool getIsGeneratingProcesses() const;
    void moveToFinished(const String& processName);

    void setIsGeneratingProcesses(bool val);

private:
    ProcessManager(long long batch_process_freq, long long min_ins, long long max_ins);
    ProcessManager(const ProcessManager&);
    ProcessManager& operator = (const ProcessManager&);
    ~ProcessManager() = default;
    static ProcessManager* sharedInstance;
    
    long long batch_process_freq;
    long long min_ins;
    long long max_ins;
    bool isGeneratingProcesses;

    std::vector<Process*> activeProcesses;
    std::vector<Process*> finishedProcesses;
};