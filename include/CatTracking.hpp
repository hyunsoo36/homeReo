#pragma once
#include <stdio.h>
#include "ImageProcessing.hpp"

class CatTracking
{
public:
    CatTracking();
    ~CatTracking();
    int IntegralMovement(float flowMap[BLOCK_COUNT_WIDTH][BLOCK_COUNT_HEIGHT]);

private:
    static float integralMap[BLOCK_COUNT_WIDTH][BLOCK_COUNT_HEIGHT];
    static float integralMax;
    static float integralMin;
};

