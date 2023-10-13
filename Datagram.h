#ifndef DATAGRAM_H
#define DATAGRAM_H

#include "Datatype.h"

class Datagram
{
public:
    const int head_len = 128;
    Datatype data_type;
    int data_len;
    int counter;
    unsigned char* header;
    char* data;
    Datagram(const char* in_data, int count, int datatype, int datalen);
    Datagram(const char* datagram);
    Datagram();
    // ~Datagram();
    void GetHeader();
    void SetHeader();
    const char* GetDatagram();
    int DatagramSize() { return head_len + data_len; }
    Datagram operator=(Datagram other);
};

#endif
