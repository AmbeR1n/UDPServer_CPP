#include "Sender.h"
#include "ProgressBar.h"
#include <cstring>
#include <unistd.h>
#include <iostream>

Sender::Sender(int _stack_size, char *recv_addr, char *port, char* _file)
{
    is_recieving = false;
    stack_size = _stack_size;
    file = File(_file);
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    // Creating socket file descriptor
    if (socketfd < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Filling receiver information
    memset(&receiver, 0, sizeof(receiver));
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(strtol(port, NULL, 10));
    receiver.sin_addr.s_addr = inet_addr(recv_addr);
    resend_list = new char[int(stack_size*0.5*sizeof(int))]{"\0"};
    datagram_stack = new Datagram*[stack_size];
}

Sender::~Sender()
{
    printf("\nFreeing memory and closing buffers\n");
    for(int i = 0; i < stack_size; i++)
        delete datagram_stack[i];
    delete[] datagram_stack;
    delete[] resend_list;
    close(socketfd);
    file.stream.close();
    printf("Done.\n");
    fflush(stdout);
}

// Closes file if one was opened and opens file at file_path path.
void Sender::SetFile(const char* path)
{
    if (file.stream.is_open())
    {
        file.stream.close();
    }
    file.stream.open(path, std::ios::in);
}

void Sender::Receive(int socket, int buff_size, char* buffer, bool* flag)
{
    *flag = true;
    recvfrom(socket, buffer, buff_size, 0, NULL, NULL);
    *flag = false;
}

void Sender::Send()
{
    if (!file.stream.is_open())
        return;
    char* buffer = new char[BUFFER];
    while (true)
    {
        file.stream.read(buffer, BUFFER);
        datagram_stack[datagram_counter%stack_size] = new Datagram((const char*)buffer, datagram_counter, 2, BUFFER);
        int size = sendto(socketfd, datagram_stack[datagram_counter%stack_size]->GetDatagram(), datagram_stack[datagram_counter%stack_size]->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
        datagram_counter++;
        //printf("%d\n", datagram_counter%stack_size);
        fflush(stdout);
        if (!is_recieving)
            receive = std::async(std::launch::async, Receive, socketfd, stack_size*0.5*sizeof(int), resend_list, &is_recieving);
        if (resend_list[0] != '\0')
            Resend();
        if (file.stream.peek() == EOF)
        {
            break;
        }
    }
    printf("Sending completed");
    fflush(stdout);
    return;
}

void Sender::Resend()
{
    for (int i = 0; i < stack_size*0.5; i++)
    {
        int resend_count = *(int*)(resend_list + i * sizeof(int));
        if (datagram_stack[resend_count%stack_size]->counter == resend_count)
        {
            sendto(socketfd, datagram_stack[resend_count%stack_size]->GetDatagram(), datagram_stack[resend_count%stack_size]->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
        }
    }
}