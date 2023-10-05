#include "ProgressBar.h"
#include <chrono>
#include <iostream>
#include <iomanip>

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

ProgressBar::ProgressBar(uint64_t size_)
{
    size = size_;
    reached = '+';
    unreached = '-';
    duration = 0;
    speed = Speed();
    progress = 0;
    progress_bar = std::string(20, unreached);
    start = current_time<std::chrono::nanoseconds>();
}

void ProgressBar::PrintLine()
{
    printf("%02lu:%02lu %.1f%% %s %.2f %s        \r", duration/static_cast<int>(Units::Nanoseconds)/60, duration/static_cast<int>(Units::Nanoseconds)%60, progress, progress_bar.c_str(), speed.Calculate(), speed.unit.c_str());
    fflush(stdout);
}

void ProgressBar::PrintFinal()
{
    double final_speed = (double)progress/100*size/duration*static_cast<int>(Units::Nanoseconds);
    printf("%02lu:%02lu %.1f%% %s %.5f %s\n", duration/static_cast<int>(Units::Nanoseconds)/60, duration/static_cast<int>(Units::Nanoseconds)%60, progress, 
                                            progress_bar.c_str(), final_speed / Conversion_Factor(final_speed), Conversion_Unit(final_speed).c_str());
}
// ds - sent data's size
void ProgressBar::Update(int ds, int* temp_size)
{
    //uint64_t prev_duration = duration;
    duration = current_time<std::chrono::nanoseconds>() - start;
    //double dt = duration - prev_duration;

    progress += (double)ds/size*100;
    progress_bar[(int)progress / 5] = reached;

    if (temp_size != nullptr)
    {
        double rate = (double)*(temp_size);
        speed = Speed(rate, Conversion_Factor(rate), Conversion_Unit(rate));
    }
}

double ProgressBar::Progress()
{
    return progress;
}
