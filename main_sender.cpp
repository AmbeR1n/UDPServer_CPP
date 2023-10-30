#include "Sender.h"
#include <iostream>

int main(int argc, char* argv[])
{ 
    if (argc != 5)
    {
        std::cout << "Requires stack size, server address, server port and path to file";
    }
    int stack_size = strtol(argv[1], nullptr, 10);
    char* recv_addr = argv[2];
    char* recv_port = argv[3];
    char* file_path = argv[4];
    File file = File(file_path);
    Sender* sender = new Sender(stack_size, recv_addr, recv_port);
    char buffer[25152-128];
    int counter = 0;
    while (file.stream.peek() != EOF)
    {
        file.stream.read(buffer, 25152-128);
        Datagram* datagram = new Datagram((const char*)buffer, counter, 3, strlen(buffer));
        sender->SendDatagram(datagram);
        counter++;
    }
    sender->SendFile(file_path);
    delete sender;
    return 1;
}