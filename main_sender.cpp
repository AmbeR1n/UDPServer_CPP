#include "Sender.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{ 

    if (argc != 5)
    {
        std::cout << "sender stack_size server_address server_port file_path";
    }
    
    int stack_size = strtol(argv[1], nullptr, 10);
    char* recv_addr = argv[2];
    char* recv_port = argv[3];
    char* file_path = argv[4];

    Sender* sender = new Sender(stack_size, recv_addr, recv_port);
    sender->SendFile(file_path);
    // int current_buffer = 20;
    // File file = File(file_path);
    // char buffer[BUFFER];
    // int counter = 0;
    // while (file.stream.peek() != EOF)
    // {
    //     if (current_buffer > MAX_BUFFER)
    //         current_buffer = MAX_BUFFER;
    //     file.stream.read(buffer, current_buffer);
    //     Datagram* datagram = new Datagram((const char*)buffer, counter, 3, current_buffer);
    //     sleep(1);
    //     int size = sender->SendDatagram(datagram);    
    //     counter++;
    //     if (counter%2)
    //         current_buffer += 5000;
    //     else
    //         current_buffer += 1;
    // }
    delete sender;
    return 1;
}