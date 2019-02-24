#pragma once


void getFileNameFromDate(char *buf, int len);

template <typename T>
class NumUtil 
{
public:
    void setMinimum(const T* input, T* originalMin)
    {
        if(*input < *originalMin)
        {
            *originalMin = *input;
        }
    }
    void setMaximum(const T* input, T* originalMax)
    {
        if(*input > *originalMax)
        {
            *originalMax = *input;
        }
    }

private:


};
