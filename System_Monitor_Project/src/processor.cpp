#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float _activeJiffies = static_cast<float>(LinuxParser::ActiveJiffies());
    float _totalJiffies = static_cast<float>(LinuxParser::Jiffies());
    return (_activeJiffies > 0) ? (_activeJiffies / _totalJiffies) : 0;
    }