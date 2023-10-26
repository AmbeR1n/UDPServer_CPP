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
    char* file = argv[4];
    
    Sender* sender = new Sender(stack_size, recv_addr, recv_port);
    sender->SendFile(file);
    delete sender;
    return 1;
}