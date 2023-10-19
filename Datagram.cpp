#include "Datagram.h"
#include <cstring>
#include <iostream>

// Building datagram from given header info and data
Datagram::Datagram(const char *in_data, int count, int datatype, int datalen)
{
    counter = count;
    data_type = (Datatype)datatype;
    data_len = datalen;
    data = new char[data_len];
    header = new char[head_len];
    memcpy(data, in_data, data_len);
    SetHeader();
}
// Separate header and data from a given datagram
Datagram::Datagram(const char *datagram)
{
    header = new char[head_len];
    memcpy(header, datagram, head_len);
    GetHeader();
    data = new char[data_len];
    memcpy(data, datagram + head_len, data_len);
}

void Datagram::SetHeader()
{
    memcpy(header, &counter, sizeof counter);
    memcpy(header + sizeof counter, &data_type, sizeof data_type);
    memcpy(header + sizeof counter + sizeof data_type, &data_len, sizeof data_len);
}

void Datagram::GetHeader()
{
    counter = *(int *)header;
    data_type = (Datatype)(*(int *)(header + sizeof counter));
    data_len = *(int *)(header + sizeof counter + sizeof data_type);
}

Datagram::~Datagram()
{
    delete[] data;
    delete[] header;
}

void Datagram::SetData(char *_data, int _data_len)
{
    data_len = _data_len;
    memcpy(data, _data, data_len);
}

char *Datagram::GetData()
{
    return data;
}

const char *Datagram::GetDatagram()
{
    std::cout << header << std::endl;
    return (std::string(header) + data).c_str();
}

int Datagram::DatagramSize()
{
    return head_len + data_len; 
}

Datagram Datagram::operator=(Datagram other)
{
    return Datagram(other.data, other.counter, (int)other.data_type, other.data_len);
}
