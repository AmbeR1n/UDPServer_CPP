#ifndef SPEED_H
#define SPEED_H

#include <cstring>

struct Speed
{
    double value;
    double unit_factor;
    std::string unit;
    void Update(double _value, double _unit_factor, std::string _unit)
    {
        value = _value;
        unit_factor = _unit_factor;
        unit = _unit;  
   }
    Speed(double v, double uf, std::string u)
    {
        value = v;
        unit_factor = uf;
        unit = u;
    }
    Speed()
    {
        value = 0;
        unit_factor = 1;
        unit = "бит/с";
    }
    double Calculate()
    {
        return value / unit_factor;
    }
};
#endif