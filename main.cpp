#include <iostream>
#include <stdlib.h> 
#include <cstdlib>
using namespace std;

#include "InputHandler.h"

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

    while (isRunning) {
        cout << "Enter command: ";
        cin >> command;

        if (command == "initialize") {
            initialize();
        }
        // add other commands here (c++ doesn't support string with switch case)
        else if (command == "screen") {
            screen();
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
            clear();
        }
        else if (command == "exit") {
            isRunning = false;
        }
        else {
            cout << "Unknown command." << endl;
        }
    }

    exit(1);
}