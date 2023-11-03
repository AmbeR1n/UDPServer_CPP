#ifndef DATAGRAM_H
#define DATAGRAM_H

#include "Datatype.h"
#include "consts.h"

class Datagram
{
public:
    Datatype data_type;
    int data_len;
    static int global_counter;
    int counter;
    Datagram(const char* in_data, int datatype, int datalen);
    Datagram(const char* datagram);
    Datagram();
    ~Datagram();
    void SetData(const char* _data, int _data_len);
    void ResetCounter();
    char* GetData();
    const char* GetDatagram();
    int DatagramSize();
    
private:
    char* datagram;
    void GetHeader(char* data);
    void SetHeader();
};

#endif
