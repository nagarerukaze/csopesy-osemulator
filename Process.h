#include <string>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

typedef std::string String;

class Process 
{
public:
	enum ProcessState
	{
		RUNNING,	// Instructions are being executed
		WAITING,	// Process waiting for an event to occur
		READY,		// Waiting to be assigned to a processor
		TERMINATED	// Finished execution
	};

	Process();
	~Process() = default;
	Process(String name, long long totalLinesOfCode);

	void printInfo();
	void draw();
	

	// Getters
	String getName() const;
	long long getCurrentInstructionLine() const;
	long long getTotalLinesOfCode() const;
	String getTimestamp();
	ProcessState getState() const;

	int getCPUCoreID();

	// Setters
	void setState(ProcessState state);
	void setCPUCoreID(int id);

	// Process Operations
	void nextLine();

	

	

private:
	String name;
	long long currentInstructionLine;
	long long totalLinesOfCode;
	time_t timeCreated;
	ProcessState currentState;

	//
	int cpuCoreID;
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
//	bool isFinished() const;
//	int getCPUCoreID() const;
//	void setCPUCoreID(int cpuCoreID);
//	void setTimeCreated();
//	void printCommand(std::ofstream& outfile);
//	void printToTextFile();
//
//private:
//	int cpuCoreID = -1;
//
//	void draw() const;
//	void nextLine();
//
//	friend class ProcessManager;
//};