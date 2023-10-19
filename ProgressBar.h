#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H
#include <string>


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

struct Speed
{
    double value;
    double unit_factor;
    std::string unit;
    void operator=(Speed s)
    {
        this->value = s.value;
        this->unit_factor = s.unit_factor;
        this->unit = s.unit;
    }
    Speed(double v, double uf, std::string u)
    {
        this->value = v;
        this->unit_factor = uf;
        this->unit = u;
    }
    Speed()
    {
        this->value = 0;
        this->unit_factor = 1;
        this->unit = "бит/с";
    }
    double Calculate()
    {
        return value / unit_factor;
    }
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
    int start;
    int duration;
    ProgressBar(long size_, int start_time);
    void PrintLine();
    void PrintFinal();
    void Update(long sent_size, int time);
    double Progress();
};

#endif