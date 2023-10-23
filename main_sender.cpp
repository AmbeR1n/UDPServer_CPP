#include "Sender.h"
#include <iostream>

int main(int argc, char* argv[])
{ 
    if (argc != 5)
    {
        std::cout << "Requires stack size, server address, server port and path to file";
    }
    Sender* sender = new Sender(strtol(argv[1], nullptr, 10), argv[2], argv[3], argv[4]);
    sender->Send();
    delete sender;
    return 1;
}