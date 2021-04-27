#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float retVal = 0.f;
  float MemTotal = 0.f;
  float MemFree = 0.f;
  string line, key, val, val2;
  int dataCounter = 0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "MemTotal:"){
          linestream >> val;
          MemTotal = std::stof(val);
          dataCounter++;
        }
        else if (key == "MemFree:"){
          linestream >> val2;
          MemFree = std::stof(val2);
          dataCounter++;
        }
        if (dataCounter == 2){
          retVal = MemTotal - MemFree;
          retVal = (retVal > 0) ? (retVal/MemTotal) : 0.f;
          return retVal;
        }
      }
    }
  }
  return retVal; 
}

// DONE: Read and return the system uptime (in seconds)
long LinuxParser::UpTime() { 
  string line, sysUptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> sysUptime;
    return std::stol(sysUptime);
  }
  return 0; 
}

// DONE: Read and return the number of jiffies for the system
// Using aggregate CPU information
long LinuxParser::Jiffies() { 
  string line, key, val;
  long retValue = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> key){
      if (key == "cpu"){
        while (linestream >> val){
          retValue += std::stol(val);
        }
        return retValue;
      }
    }
  }
  return retValue;
}

// DONE: Read and return the number of active jiffies for a PID
// #14 utime - CPU time spent in user code, measured in clock ticks
// #15 stime - CPU time spent in kernel code, measured in clock ticks
// #16 cutime - Waited-for children's CPU time spent in user code (in clock ticks)
// #17 cstime - Waited-for children's CPU time spent in kernel code (in clock ticks)
long LinuxParser::ActiveJiffies(int pid) { 
  string utime, stime, cutime, cstime, line, temp;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      for (int i = 0; i < 13; i++){
        linestream >> temp;
      }
    linestream >> utime >> stime >> cutime >> cstime;
    return std::stol(utime) + std::stol(stime) + std::stol(cutime) + std::stol(cstime);
    }
  }
  return 0; 
}

// DONE: Read and return the number of active jiffies for the system
// Assuming this is 'user' + 'nice' + 'system time'
long LinuxParser::ActiveJiffies() {
  string line, key, val;
  long retValue = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu"){
          for (int i = 0; i < 3; i++){
            linestream >> val;
            retValue += std::stol(val);
          }
          return retValue;
        }
      }
    }
  }
  return 0; 
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string line, key, val, temp;
  long retValue = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu"){
          for (int i = 0; i < 3; i++){
            linestream >> temp;
          }
          linestream >> val;
          retValue = std::stol(val);
          return retValue;
        }
      }
    }
  }
  return 0; 
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  std::vector<string> cpuValues;
  string line, val, key;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu"){
          while(linestream >> val)
          {
            cpuValues.push_back(val);
          }
          return cpuValues;
        }
      }
    }
  }
  return cpuValues; 
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, val, key;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key){
        if (key == "processes"){
          linestream >> val;
          return std::stoi(val);
        }
      }
    }
  }
  return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, val, key;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "procs_running"){
          linestream >> val;
          return std::stoi(val);
        }
      }
    }
  }
  return 0;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line = "no value";
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line; 
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string key, val, line;
  int vmSize;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        // VmSize is the sum of all mapped memory
        if (key == "VmSize:"){
          linestream >> val;
          vmSize = std::stoi(val);
          vmSize = (vmSize > 0) ? (vmSize / 1024) : 0;
          return std::to_string(vmSize);
        }
      }
    }
  }
  return "error";
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string key, val, line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key){
        if (key == "Uid:"){
          linestream >> val;
          return val;
        }
      }
    }
  }
  return "error";
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string userName, x, userId, line;
  string processId = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while(std::getline(stream, line))
      {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        while (linestream >> userName >> x >> userId) {
          if (userId == processId) return userName;
        }
      }
  }
  return "error"; 
}

// DONE: Read and return the uptime of a process (in seconds)
long LinuxParser::UpTime(int pid) { 
  string line, val, temp;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i < 21; i++){
        linestream >> temp;
      }
      linestream >> val;
      return UpTime() - (std::stol(val) / sysconf(_SC_CLK_TCK));
    }
  }  
  return 0; 
}
