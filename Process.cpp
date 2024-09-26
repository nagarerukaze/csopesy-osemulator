#include "Process.h"
#include <ctime>

Process::Process() {
	std::string getTimestamp() const {
		std::ostringstream oss;
		std::tm* timeStamp = std::localtime(&timeCreated);
		oss << std::put_time(timeStamp, "(%m/%d/%Y %I:%M:%S %p)");
		return oss.str();
	}
}