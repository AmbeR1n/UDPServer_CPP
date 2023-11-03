#include "Datagram.h"
#include <cstring>

int Datagram::global_counter = -1;

// Create new datagram from data. Increments counter
Datagram::Datagram(const char *in_data, int datatype, int datalen)
{
    global_counter++;
    counter = global_counter;
    data_type = (Datatype)datatype;
    data_len = datalen;
    datagram = new char[data_len+DATAGRAM_HEADER];
    SetData(in_data, data_len);
    SetHeader();
    
}

// Build datagram from another one given in form of char array. Doesn't increment counter
Datagram::Datagram(const char *in_datagram)
{
    char* _header = new char[DATAGRAM_HEADER];
    memcpy(_header, in_datagram, DATAGRAM_HEADER);
    GetHeader(_header);
    delete[] _header;
    datagram = new char[data_len+DATAGRAM_HEADER];
    datagram = (char*)in_datagram;
}

// Empty datagram. Doesn't increment counter
Datagram::Datagram()
{
    datagram = new char[DATAGRAM_HEADER];
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

// Delete datagram
Datagram::~Datagram()
{
    //delete[] datagram;
}

// Sets given data with given datalen
void Datagram::SetData(const char *_data, int _datalen)
{
    data_len = _datalen;
    memcpy(datagram+DATAGRAM_HEADER, _data, data_len);
}

// Returns data from datagram
char *Datagram::GetData()
{
    return datagram+DATAGRAM_HEADER;
}

// Returns datagram (header+data)
const char *Datagram::GetDatagram()
{
    return datagram;
}

// Returns size of header and data
int Datagram::DatagramSize()
{
    return DATAGRAM_HEADER + data_len; 
}

// Resets counter to 0. After this, next created datagram will have counter = 0.
void Datagram::ResetCounter()
{
    counter = 0;
}