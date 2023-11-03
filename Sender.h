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
    Sender(char* recv_addr, char* port);
    ~Sender();

    void SendEnd();
    void SendFile(char* file);
    int SendDatagram(Datagram* datagram);
    int SendDatagram(const char *in_data, int datatype, int datalen);
    void ResetCounter();

private:
    std::future<void> receive;
    File file;
    struct sockaddr_in receiver;
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