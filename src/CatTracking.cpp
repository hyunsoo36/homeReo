#include "CatTracking.hpp"
#include "ImageProcessing.hpp"
#include "Util.hpp"
#include <stdio.h>


NumUtil<float> numUtil;

CatTracking::CatTracking() 
{
    integralMax = 0;
    integralMin = 987654321;
}
int CatTracking::IntegralMovement(float flowMap[BLOCK_COUNT_WIDTH][BLOCK_COUNT_HEIGHT])
{
    for(int i=0; i<BLOCK_COUNT_WIDTH; i++) 
    {
        for(int j=0; j<BLOCK_COUNT_HEIGHT; j++)
        {
            integralMap[i][j] += flowMap[i][j];
            numUtil.setMaximum(&flowMap[i][j], &integralMax);
            numUtil.setMinimum(&flowMap[i][j], &integralMin);
        }
    }
    return 0;

}

CatTracking::~CatTracking() 
{

}