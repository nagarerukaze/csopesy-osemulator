#include <string>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

class Process 
{
public:
	Process();
	~Process() = default;
	Process(std::string name, int totalLinesOfCode);

	void printInfo();
	void draw();
	void nextLine();

	// getters + setters
	std::string getName() const;
	int getCurrentInstructionLine() const;
	int getTotalLinesOfCode() const;
	std::string getTimestamp();
private:
	std::string name;
	int currentInstructionLine;
	int totalLinesOfCode;
	time_t timeCreated;
};



//#pragma once
//#include <stdlib.h>
//#include <vector>
//#include <fstream> 
//
//class Process
//{
//public:
//
//	enum ProcessState
//	{
//		RUNNING,	// Instructions are being executed
//		WAITING,	// Process waiting for an event to occur
//		READY,		// Waiting to be assigned to a processor
//		TERMINATED	// Finished execution
//	};
//
//	void setState(ProcessState state);
//	ProcessState getState() const;
//	bool isFinished() const;
//	int getCPUCoreID() const;
//	void setCPUCoreID(int cpuCoreID);
//	void setTimeCreated();
//	void terminate();
//	void printCommand(std::ofstream& outfile);
//	void printToTextFile();
//
//private:
//	ProcessState currentState;
//	int cpuCoreID = -1;
//
//	void draw() const;
//	void nextLine();
//
//	friend class ProcessManager;
//};