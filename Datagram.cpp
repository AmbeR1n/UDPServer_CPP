#include "Datagram.h"
#include <cstring>
#include <iostream>

// Building datagram from given data and info
Datagram::Datagram(const char *in_data, int count, int datatype, int datalen)
{
    counter = count;
    data_type = (Datatype)datatype;
    data_len = datalen;
    datagram = new char[data_len+head_len];
    SetData(in_data, data_len);
    SetHeader();
}
// Separate header and data from a given datagram
Datagram::Datagram(const char *in_datagram)
{
    datagram = new char[data_len+head_len];
    datagram = (char*)in_datagram;
    GetHeader();
}

void Datagram::SetHeader()
{
    memcpy(datagram, &counter, sizeof counter);
    memcpy(datagram + sizeof counter, &data_type, sizeof data_type);
    memcpy(datagram + sizeof counter + sizeof data_type, &data_len, sizeof data_len);
}

void Datagram::GetHeader()
{
    memcpy(&counter, datagram, sizeof counter);
    memcpy(&data_type, datagram + sizeof counter, sizeof data_type);
    memcpy(&data_len, datagram + sizeof counter + sizeof data_type, sizeof data_len);
    //counter = *(int *)header;
    //data_type = (Datatype)(*(int *)(header + sizeof counter));
    //data_len = *(int *)(header + sizeof counter + sizeof data_type);
}

Datagram::~Datagram()
{
    //delete[] datagram;
}

void Datagram::SetData(const char *_data, int _datalen)
{
    data_len = _datalen;
    memcpy(datagram+head_len, _data, data_len);
}

char *Datagram::GetData()
{
    return datagram+head_len;
}

const char *Datagram::GetDatagram()
{
    return datagram;
}

int Datagram::DatagramSize()
{
    return head_len + data_len; 
}

Datagram Datagram::operator=(Datagram other)
{
    return Datagram(other.GetData(), other.counter, (int)other.data_type, other.data_len);
}
