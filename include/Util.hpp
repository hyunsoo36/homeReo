#pragma once


void getFileNameFromDate(char *buf, int len);

template <typename T>
class NumUtil 
{
public:
    static void setMinimum(const T* input, T* originalMin);
    static void setMaximum(const T* input, T* originalMax);

private:


}
