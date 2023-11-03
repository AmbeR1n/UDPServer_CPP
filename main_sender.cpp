#include "Sender.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{ 

    if (argc != 4)
    {
        std::cout << "sender server_address server_port file_path";
    }
    
    char* recv_addr = argv[1];
    char* recv_port = argv[2];
    char* file_path = argv[3];

    Sender* sender = new Sender(recv_addr, recv_port);
    //sender->SendFile(file_path);
    File file = File(file_path);
    char buffer[BUFFER+1];
    buffer[BUFFER] = '\0';
    int counter = 0;
    std::ofstream stream;
    stream.open("upload/test.txt", std::ios::out);
    while (file.stream.peek() != EOF)
    {
        if (counter == 100)
            break;
        file.stream.read(buffer, BUFFER);
        Datagram* datagram = new Datagram((const char*)buffer, 3, strlen(buffer));
        std::string new_line = std::to_string(counter)+buffer+"\n";
        stream.write(new_line.c_str(), new_line.size());
        std::cout << "Sent datagram #" << datagram->counter << std::endl;
        sender->SendDatagram(datagram);
        counter++;
    }
    sender->SendEnd();
    file.stream.close();
    delete sender;
    return 1;
}