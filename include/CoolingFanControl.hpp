#ifndef _COOLINGFANCONTROL_HPP__
#define _COOLINGFANCONTROL_HPP__
#include <stdio.h>

class CoolingFanControl
{
public:
    CoolingFanControl();
    ~CoolingFanControl();
    double getCPUtemperature();
    int PIDcontrol();
private:
    FILE *temperatureFile;
    double cpu_temp;
};

#endif
