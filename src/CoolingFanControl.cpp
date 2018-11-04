#include "CoolingFanControl.hpp"
#include <stdio.h>

#define CPU_TEMPERATURE_FILE_NAME "/sys/class/thermal/thermal_zone0/temp"
#define MAX_CPU_TEMPERATURE 100.0

CoolingFanControl::CoolingFanControl() {
    temperatureFile = fopen (CPU_TEMPERATURE_FILE_NAME, "r");
    if (temperatureFile == NULL) {
        printf("cannot open file : %s", CPU_TEMPERATURE_FILE_NAME);
    }
}

CoolingFanControl::~CoolingFanControl() {
    if (temperatureFile != NULL) {
        fclose (temperatureFile);
    }
}

double CoolingFanControl::getCPUtemperature() 
{
    if(temperatureFile == NULL) {
        return MAX_CPU_TEMPERATURE;
    }
    fscanf (temperatureFile, "%lf", &cpu_temp);
    cpu_temp /= 1000;
    //printf ("The temperature is %6.3f C.\n", T);

    return cpu_temp;
}
