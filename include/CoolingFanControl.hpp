#pragma once
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

