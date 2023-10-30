#include "Sender.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{ 

    if (argc != 5)
    {
        std::cout << "Requires stack size, server address, server port and path to file";
    }
    int MAX_BUFFER = 60*1024;
    int stack_size = strtol(argv[1], nullptr, 10);
    char* recv_addr = argv[2];
    char* recv_port = argv[3];
    char* file_path = argv[4];
    int current_buffer = 100;
    File file = File(file_path);
    Sender* sender = new Sender(stack_size, recv_addr, recv_port);
    char buffer[MAX_BUFFER];
    int counter = 0;
    while (file.stream.peek() != EOF)
    {
        file.stream.read(buffer, current_buffer);
        Datagram* datagram = new Datagram((const char*)buffer, counter, 3, current_buffer);
        sender->SendDatagram(datagram);
        std::cout << "Sent " << current_buffer << " bytes. Waiting 1 second and sending bigger packet\n";
        counter++;
        current_buffer += 100;
        sleep(1);
    }
    //sender->SendFile(file_path);
    delete sender;
    return 1;
}