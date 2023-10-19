#ifndef RECEIVER_H
#define RECEIVER_H
#include "File.h"
#include "arpa/inet.h"

class Receiver
{
public:
    void Receive();
    Receiver(char* _port);
    ~Receiver();
private:
    const int BUFFER = 60*1024;
    int socketfd;
    File file;
    struct sockaddr_in receiver;
    struct sockaddr_in sender;
    socklen_t sender_len;
    
    void Send(char* data);
};
#endif