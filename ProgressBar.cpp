#include "ProgressBar.h"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <cstring>

template<typename T>
uint64_t current_time()
{
    return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();
}

double ProgressBar::Conversion_Factor(double value)
{
    double factor;

    if (value > 1024.0 * 1024 * 1024 / 8)
    {
        factor = static_cast<int>(Units::Gigabit);
    }
    else if (value > 1024.0 * 1024 / 8)
    {
        factor = static_cast<int>(Units::Megabit);
    }
    else if (value > 1024.0 / 8)
    {
        factor = static_cast<int>(Units::Kilobit);
    }
    else
    {
        factor = 1;
    }
    return factor;
}

std::string ProgressBar::Conversion_Unit(double value)
{
    std::string unit;
    if (value > 1024.0 * 1024 * 1024 / 8)
    {
        unit = "Гбит/с";
    }
    else if (value > 1024.0 * 1024 / 8)
    {
        unit = "Мбит/с";
    } 
    else if (value > 1024.0 / 8)
    {
        unit = "Кбит/с";
    }
    else
    {
        unit = "бит/с";
    }
    return unit;
}

ProgressBar::ProgressBar(long size_)
{
    size = size_;
    reached = '+';
    unreached = '-';
    duration = 0;
    speed = Speed();
    progress = 0;
    progress_bar = new char[20];
    memset(progress_bar, unreached, 20);
    start = current_time<std::chrono::nanoseconds>();
}

void ProgressBar::PrintLine()
{
    printf("%02lu:%02lu\t%.1f%%\t%s\t%.2f %s\t%lu        \r", duration/static_cast<int>(Units::Nanoseconds)/60, duration/static_cast<int>(Units::Nanoseconds)%60, progress, (const char*)progress_bar, speed.Calculate(), speed.unit.c_str(), size);
    fflush(stdout);
}

void ProgressBar::PrintFinal()
{
    double final_speed = (double)progress/100*size/duration*static_cast<int>(Units::Nanoseconds);
    printf("%02lu:%02lu\t%.1f%%\t%s\t%.5f %s\n", duration/static_cast<int>(Units::Nanoseconds)/60, duration/static_cast<int>(Units::Nanoseconds)%60, progress, 
                                            (const char*)progress_bar, final_speed / Conversion_Factor(final_speed), Conversion_Unit(final_speed).c_str());
}

void ProgressBar::Update(long temp_size)
{
    duration = current_time<std::chrono::nanoseconds>() - start;

    progress += (double)temp_size/size*100;
    memset(progress_bar, reached, (int)progress / 5);
    double rate = (double)temp_size;
    speed = Speed(rate, Conversion_Factor(rate), Conversion_Unit(rate));
}

double ProgressBar::Progress()
{
    return progress;
}
