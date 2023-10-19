#include "Sender.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{ 
    Sender* sender = new Sender(strtol(argv[1], nullptr, 10), argv[2], argv[3], argv[4]);
    sender->Send();
    delete sender;
    exit(0);
    return 1;
}