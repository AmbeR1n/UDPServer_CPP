#include "Sender.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{ 
    if (argc != 5)
    {
        std::cout << "Requires stack size, server address, server port and path to file";
    }
    std::cout << "Before constructor" << std::endl;
    Sender* sender = new Sender(strtol(argv[1], nullptr, 10), argv[2], argv[3], argv[4]);
    std::cout << "After constructor" << std::endl;
    sender->Send();
    delete sender;
    exit(0);
    return 1;
}