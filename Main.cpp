#include <iostream>
#include <stdio.h>

#include <pthread.h>
#include <unistd.h>
#include "CoolingFanControl.hpp"
#include "Util.hpp"
#include "ImageProcessing.hpp"
// #include "wiringPi.h"
// #include "wiringSerial.h"



int main ( int argc,char **argv ) 
{
    ImageProcessing imageProcessingInst;
    pthread_t imageProcessingThread;
    if(pthread_create(&imageProcessingThread, NULL, imageProcessingInst.imageProcessingThread, NULL)) 
    {
		printf("Cannot creating thread - imageProcessing\n");
		return -1;
	}

    CoolingFanControl fanController;
    printf("CPU temperature : %lf\n", fanController.getCPUtemperature());

    while(1) {
        sleep(1);
    }
    
}