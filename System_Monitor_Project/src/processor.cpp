#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
    long _activeJiffies = LinuxParser::ActiveJiffies();
    _activeJiffies = (_activeJiffies == 0) ? 0 : (_activeJiffies / LinuxParser::Jiffies());
    return float(_activeJiffies); 
    }