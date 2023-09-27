#include "ProgressBar.h"
#include <chrono>
#include <iostream>
#include <iomanip>
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

template<typename T>
long current_time()
{
    return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();
}

ProgressBar::ProgressBar(uint64_t size)
{
    this->size = size;
    this->reached = '+';
    this->unreached = '-';
    this->duration = 0;
    this->speed = 0;
    this->progress = 0;
    this->progress_bar = std::string(20, unreached);
    this->start = current_time<std::chrono::milliseconds>();
}

void ProgressBar::PrintLine()
{
    printf("%d %.1f%% %s %.2f Гбит/s\t\t\r", this->duration, this->progress, this->progress_bar.c_str(), this->speed);
    fflush(NULL);
}

void ProgressBar::PrintFinal()
{
    printf("%d %.1f%% %s %.5f Гбит/s\n", this->duration, this->progress, this->progress_bar.c_str(), (double)this->size/this->duration*static_cast<int>(Units::Millisecond)/static_cast<int>(Units::Gigabit));
}

void ProgressBar::Update(uint64_t current_size)
{
    this->duration = current_time<std::chrono::milliseconds>() - this->start;
    this->progress = (double)current_size/this->size*100;
    this->speed = (double)current_size/(this->duration)*static_cast<int>(Units::Millisecond)/static_cast<int>(Units::Gigabit);
    this->progress_bar[(int)progress / 5] = this->reached;
}

