#ifndef SENDER_H
#define SENDER_H
#include "arpa/inet.h"
#include <fstream>
#include <future>
#include "Datagram.h"
#include <cstring>
#include "File.h"

class Sender
{
public:
    Sender(int _stack_size, char* recv_addr, char* port);
    Sender(const Sender& other);
    Sender(Sender&& other);
    Sender& operator=(const Sender& other);
    Sender& operator=(Sender&& other);
    Sender& operator=(Sender other);
    ~Sender();


    void SendFile(char* file);
    void SendDatagram(Datagram* datagram);
    void SendDatagram(const char *in_data, int datatype, int datalen);
    void ResetCounter();

private:

    const int BUFFER = 24*1024+512+256-128-64+32-16 - 128; //128 b - header size 
    std::future<void> receive;
    File file;
    struct sockaddr_in receiver;
    int stack_size;
    int datagram_counter = 2;
    int resend_counter = 0;
    int socketfd;
    bool ready_to_resend;
    bool is_recieving;
    char* resend_list;
    Datagram** datagram_stack;
    static void Receive(int socket, int buff_size, char* buffer, bool* flag_resend);
    void StartAsyncRecv();
    void Resend();
};
#endif