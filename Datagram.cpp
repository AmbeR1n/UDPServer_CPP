#include "Datagram.h"
#include <cstring>

// Building datagram from given data and info
Datagram::Datagram(const char *in_data, int count, int datatype, int datalen)
{
    counter = count;
    data_type = (Datatype)datatype;
    data_len = datalen;
    datagram = new char[data_len+HEADER];
    SetData(in_data, data_len);
    SetHeader();
}
// Separate header and data from a given datagram
Datagram::Datagram(const char *in_datagram)
{
    char _header[HEADER];
    memcpy(_header, in_datagram, HEADER);
    GetHeader(_header);
    datagram = new char[data_len+HEADER];
    datagram = (char*)in_datagram;
}

Datagram::Datagram()
{
    datagram = new char[HEADER];
}

void Datagram::SetHeader()
{
    void* next = mempcpy(datagram, &counter, sizeof counter);
    next = mempcpy(next, &data_type, sizeof data_type);
    memcpy(next, &data_len, sizeof data_len);
}

void Datagram::GetHeader(char* data)
{
    memcpy(&counter, data, sizeof counter);
    memcpy(&data_type, data + sizeof counter, sizeof data_type);
    memcpy(&data_len, data + sizeof counter + sizeof data_type, sizeof data_len);
}

Datagram::~Datagram()
{
    //delete[] datagram;
}

void Datagram::SetData(const char *_data, int _datalen)
{
    data_len = _datalen;
    memcpy(datagram+HEADER, _data, data_len);
}

char *Datagram::GetData()
{
    return datagram+HEADER;
}

const char *Datagram::GetDatagram()
{
    return datagram;
}

int Datagram::DatagramSize()
{
    return HEADER + data_len; 
}

Datagram Datagram::operator=(Datagram other)
{
    return Datagram(other.GetData(), other.counter, (int)other.data_type, other.data_len);
}
