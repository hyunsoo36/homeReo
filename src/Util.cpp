#include "Util.hpp"
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <ctime>
#include <unistd.h>
#include <math.h>


void getFileNameFromDate(char *buf, int len) {
    time_t curr_time;
    struct tm *curr_tm;
    curr_time = time(NULL);
    curr_tm = localtime(&curr_time);
    snprintf(buf, len, "%02d%02d%02d_%02d%02d%02d_homeReo.avi", curr_tm->tm_year-100, curr_tm->tm_mon + 1, curr_tm->tm_mday
                            , curr_tm->tm_hour, curr_tm->tm_min, curr_tm->tm_sec);
}
template <typename T>
void NumUtil<T>::setMinimum(const T* input, T* originalMin)
{
    if(*input < *originalMin)
    {
        *originalMin = *input;
    }
}
template <typename T>
void NumUtil<T>::setMaximum(const T* input, T* originalMax)
{
    if(*input > *originalMax)
    {
        *originalMax = *input;
    }
}
