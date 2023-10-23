#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H
#include <string>
#include "Speed.h"

enum class Units
{
    //from bytes to
    Gigabit = 1024*1024*1024/8,
    Megabit = 1024*1024/8,
    Kilobit = 1024/8,
    Gigabyte = 1024*1024*1024,
    Megabyte = 1024*1024,
    Kilobyte = 1024,
    //to seconds from 
    Millisecond = 1000,
    Microsecond = 1000*1000,
    Nanoseconds = 1000*1000*1000
};

class ProgressBar
{
private:
    int count;
    char reached;
    char unreached;
    double progress;
    double sum_speed;
    long size;
    Speed speed;
    char* progress_bar;
    double Conversion_Factor(double value);
    std::string Conversion_Unit(double value);
public:
    long start;
    int duration;
    ProgressBar();
    ProgressBar(long size_, long start_time);
    void PrintLine();
    void PrintFinal();
    void Update(long sent_size, int time);
    double Progress();
};

#endif