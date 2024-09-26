#pragma once
#include <iostream>

class InputHandler
{
public:
	void printHeader();
    void initialize();
    void screen();
    void schedulerTest();
    void schedulerStop();
    void reportUtil();
    void clear();
    void newProcess(std::string name, int linesOfCode);
};
