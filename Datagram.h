#ifndef DATAGRAM_H
#define DATAGRAM_H

#include "Datatype.h"
#include "consts.h"

class Datagram
{
public:
    Datatype data_type;
    int data_len;
    int counter;
    Datagram(const char* in_data, int count, int datatype, int datalen);
    Datagram(const char* datagram);
    Datagram();
    ~Datagram();
    void SetData(const char* _data, int _data_len);
    char* GetData();
    const char* GetDatagram();
    int DatagramSize();
    Datagram operator=(Datagram other);
    
private:
    //const int HEADER = 128;
    char* datagram;
    void GetHeader(char* data);
    void SetHeader();
    
};

#endif
