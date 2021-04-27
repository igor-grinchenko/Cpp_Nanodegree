#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process (int pid_) : pid(pid_) {}

// DONE: Return this process's ID
int Process::Pid() { return pid; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() { 
    float _elapsedTime = float(LinuxParser::UpTime() - LinuxParser::UpTime(pid));    // in seconds
    float _activeJiffies = float(LinuxParser::ActiveJiffies(pid) / sysconf(_SC_CLK_TCK));
    return _elapsedTime / _activeJiffies; 
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid); }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::Uid(pid); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return LinuxParser::UpTime(pid) > LinuxParser::UpTime(a.pid); }