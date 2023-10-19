#ifndef DATAGRAM_H
#define DATAGRAM_H

#include "Datatype.h"

class Datagram
{
public:
    Datatype data_type;
    int data_len;
    int counter;
    Datagram(const char* in_data, int count, int datatype, int datalen);
    Datagram(const char* datagram);
    ~Datagram();
    void SetData(char* _data, int _data_len);
    char* GetData();
    const char* GetDatagram();
    int DatagramSize();
    Datagram operator=(Datagram other);
private:
    const int head_len = 128;
    char* header;
    char* data;
    void GetHeader();
    void SetHeader();
};

#endif
