#include "Receiver.h"
#include "Datagram.h"
#include <cstring>
#include <unistd.h>
#include <iostream>

Receiver::Receiver(char* _port)
{
    memset(&sender, 0, sizeof sender);
    memset(&receiver, 0, sizeof receiver);
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(strtol(_port, NULL, 10));
    receiver.sin_addr.s_addr = INADDR_ANY;
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
}

Receiver::~Receiver()
{
    file.stream.close();
    close(socketfd);
}

void Receiver::Send(char* data)
{

}

void Receiver::Receive()
{
    char* recv_data = new char[BUFFER];
    int current_dgram = 0;
    int previous_dgram = 0;
    printf("Waiting for data to be sent\n");
    while (true)
    {
        std::cout << socketfd << std::flush;
        recvfrom(socketfd, recv_data, BUFFER, 0, (struct sockaddr*)(&sender), &sender_len);
        
        if (strcmp(recv_data, "<END>"))
            break;
        Datagram* datagram = new Datagram(recv_data);
        if (datagram->data_type == Filename)
        {
            file.path = datagram->GetData();
            continue;
        }
        if (datagram->data_type == Filesize)
        {
            file.size = *(int*)datagram->GetData();
            continue;
        }
        current_dgram = datagram->counter;
        if (previous_dgram - current_dgram > 1)
        {
            char resend_request[9];
            memcpy(resend_request, &previous_dgram, sizeof previous_dgram);
            memcpy(resend_request + sizeof previous_dgram + 1, &current_dgram, sizeof current_dgram);
            memset(resend_request + sizeof previous_dgram, (char)58, 1);
            Send(resend_request);
        }
        printf("%d\n", datagram->counter);
        delete datagram;
    }
    delete[] recv_data;
}