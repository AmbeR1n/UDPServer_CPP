#ifndef SENDER_H
#define SENDER_H
#include "arpa/inet.h"
#include <fstream>
#include <future>
#include "Datagram.h"
#include <cstring>
#include <filesystem>

struct File
{
    std::ifstream stream;
    long size;
    char* path;
    File() {size = 0; path = (char*)"";};
    File(char* _path) 
    {
        path = _path;
        stream.open(_path);
        size = std::filesystem::file_size(path);
    }
};

class Sender
{
public:
    Sender(int _stack_size, char* recv_addr, char* port, char* _path);
    ~Sender();
    void SetFile(const char* file_path);
    void Send();
    

private:
    const int BUFFER = 60*1024;
    std::future<void> receive;
    File file;
    struct sockaddr_in receiver;
    int stack_size;
    int datagram_counter = 0;
    int socketfd;
    bool is_recieving;
    char* resend_list;
    Datagram** datagram_stack;
    static void Receive(int socket, int buff_size, char* buffer, bool* flag);
    void Resend();
};
#endif