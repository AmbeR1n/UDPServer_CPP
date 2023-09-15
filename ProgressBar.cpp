#include "ProgressBar.h"
#include <chrono>

template<typename T>
long current_time()
{
    return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void ProgressBar::CreateBar()
{
    for (int i = 0; i < this->bar_size; i++)
    {
        this->progress_bar[i] = this->unreached;
    }
}

ProgressBar::ProgressBar()
{
    this->reached = '+';
    this->unreached = '-';
    this->size = 0;
    this->bar_size = 0;
    this->current_size = 0;
    this->speed = 0;
    this->progress = 0;
    CreateBar();
    this->start = current_time<std::chrono::seconds>();
}

ProgressBar::ProgressBar(char reached, char unreached, int size)
{
    this->reached = reached;
    this->unreached = unreached;
    this->size = size;
    this->current_size = 0;
    this->speed = 0;
    this->progress = 0;
    CreateBar();
    this->start = current_time<std::chrono::seconds>();
}

void ProgressBar::PrintLine()
{
}

void ProgressBar::Update(int sent_size, long nanoseconds)
{
    int duration = current_time<std::chrono::seconds>() - this->start;
    this->current_size += sent_size;
    this->progress = (double)this->current_size/this->size*100;
    this->speed = sent_size/(current_time<std::chrono::nanoseconds>() - nanoseconds);
    
}

