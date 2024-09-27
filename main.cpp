#include <iostream>
#include <stdlib.h> 
#include <cstdlib>
using namespace std;

#include "ProcessManager.cpp"
#include "Process.cpp"

void printHeader() {
    // put ascii art here
    cout << "  ____ ____   ___  ____  _____ ______ __   __" << endl;
    cout << " / ___/ ___| / _ \\|  _ \\| ____/ ___| \\ \\ / /" << endl;
    cout << "| |   \\___ \\| | | | |_) |  _| \\___/   \\ V / " << endl;
    cout << "| |___ ___) | |_| |  __/| |___ ___) | |   |    " << endl;
    cout << " \\____|____/ \\___/|_|   |_____|____/  |___|  " << endl;
    cout << "Hello, Welcome to CSOPESY commandline!" << endl;

    cout << "Type 'exit', to quit, 'clear' to clear the screen." << endl;
}

void initialize() {
    // initialize command code here
    cout << "\"initialize\" command recognized. Doing something..." << endl;
}

void screen() {
    // screen command code here
    cout << "\"screen\" command recognized. Doing something..." << endl;
}

void schedulerTest() {
    // scheduler-test command code here
    cout << "\"scheduler-test\" command recognized. Doing something..." << endl;
}

void schedulerStop() {
    // scheduler-stop command code here
    cout << "\"scheduler-stop\" command recognized. Doing something..." << endl;
}

void reportUtil() {
    // report-util command code here
    cout << "\"report-util\" command recognized. Doing something..." << endl;
}

void clear() {
    // clear command code here
    system("cls"); // !! CHANGE TO "cls" FOR WINDOWS !!
    printHeader();
}

int main() {
    bool isRunning = true;
    string command;
    
    clear();

    ProcessManager::initialize();

    ProcessManager::getInstance()->createProcess("process1001", 100);
    ProcessManager::getInstance()->createProcess("process1002", 200);
    ProcessManager::getInstance()->createProcess("process1003", 300);

    ProcessManager::getInstance()->displayProcess("process1001");
    ProcessManager::getInstance()->displayProcess("process1004");

    
}