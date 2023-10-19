#include "Sender.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{ 
    if (argc != 5)
    {
        argv = new char*[5]{argv[0], (char*)"100", (char*)"127.0.0.1", (char*)"3333", (char*)"upload/train.txt"};
        //printf("Requires buffer size, reciever address, port and file path e.g.: ./sender 100 127.0.0.1 5000 upload/train.txt\n");
        //return -1;
    }
    Sender* sender = new Sender(strtol(argv[1], nullptr, 10), argv[2], argv[3], argv[4]);
    sender->Send();
    delete sender;
    delete[] argv;
    exit(0);
    return 1;
}