#include "Sender.h"
#include "ProgressBar.h"
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <filesystem>

Sender::Sender(int _stack_size, char *recv_addr, char *port, char* _file)
{
    std::cout << "1" << std::endl;
    char* folder = (char*)"upload/";
    is_recieving = false;
    stack_size = _stack_size;
    std::cout << "2" << std::endl;
    file = File((char*)std::filesystem::path(std::string(folder)+_file).c_str());
    std::cout << "3" << std::endl;
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    std::cout << "4" << std::endl;
    // Creating socket file descriptor
    if (socketfd < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "5" << std::endl;
    // Filling receiver information
    memset(&receiver, 0, sizeof receiver);
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(strtol(port, NULL, 10));
    receiver.sin_addr.s_addr = inet_addr(recv_addr);
    resend_list = new char[int(stack_size*0.5*sizeof(int))]{"\0"};
    datagram_stack = new Datagram*[stack_size];
    std::cout << "6" << std::endl;
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
    int size;
    char* buffer = new char[BUFFER];
    {
        char file_data[sizeof file.size];
        memcpy(file_data, (char*)&file.size, sizeof file.size);
        std::unique_ptr<Datagram> name_datagram(new Datagram(file.path.filename().c_str(), 0, 0, strlen(file.path.filename().c_str())));
        size = sendto(socketfd, name_datagram->GetDatagram(), name_datagram->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
        printf("File name sent\n");
        std::unique_ptr<Datagram> size_datagram(new Datagram((const char*)file_data, 1, 1, strlen((const char*)file_data)));
        size = sendto(socketfd, size_datagram->GetDatagram(), size_datagram->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
        printf("File size sent\n");
    }
    while (true)
    {
        file.stream.read(buffer, BUFFER);
        datagram_stack[datagram_counter%stack_size] = new Datagram((const char*)buffer, datagram_counter, 2, BUFFER);
        size = sendto(socketfd, datagram_stack[datagram_counter%stack_size]->GetDatagram(), datagram_stack[datagram_counter%stack_size]->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof(receiver));
        printf("%d, %d\n", socketfd, size);
        datagram_counter++;
        fflush(stdout);
        if (file.stream.peek() != EOF)
        {
            if (!is_recieving)
                receive = std::async(std::launch::async, Receive, socketfd, stack_size*0.5*sizeof(int), resend_list, &is_recieving);
            if (resend_list[0] != '\0')
                Resend();
            continue;
        }
        break;
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