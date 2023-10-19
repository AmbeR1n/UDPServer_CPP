#include "Receiver.h"
#include <iostream>

int main(int argc, char* argv[])
{ 
    Receiver* receiver = new Receiver(argv[1]);
    receiver->Receive();
    delete receiver;
    return 1;
}