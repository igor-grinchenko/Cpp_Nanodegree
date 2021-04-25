#include <string>
#include "format.h"

using std::string;

#define HOUR    3600    //seconds
#define MINUTE  60      //seconds

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    if (seconds == 0) return "bad data";
    long hours = seconds/HOUR;
    long remSeconds = seconds % HOUR;
    long minutes = (remSeconds != 0) ? (remSeconds/MINUTE) : 0;
    long outSeconds = remSeconds % MINUTE;
    char time [9];
    sprintf(time, "%.2ld:%.2ld:%.2ld", hours, minutes, outSeconds);
    return time; 
}