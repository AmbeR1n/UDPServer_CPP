#include "Sender.h"
#include "ProgressBar.h"
//#include "File.h"
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
    
    std::cout << "5" << std::endl;
    // Filling receiver information
    memset(&receiver, 0, sizeof receiver);
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(strtol(port, NULL, 10));
    receiver.sin_addr.s_addr = inet_addr(recv_addr);
    if (socketfd < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

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
    std::cout << "Sending to " << inet_ntoa(receiver.sin_addr) << ":" << htons(receiver.sin_port) << std::endl;
    {
        Datagram* name_datagram = (new Datagram(file.path.filename().c_str(), 0, 0, (int)strlen(file.path.filename().c_str()))); 
        size = sendto(socketfd, name_datagram->GetDatagram(), name_datagram->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
        
        char file_size[sizeof file.size];
        memcpy(file_size, (char*)&file.size, sizeof file.size);
        Datagram* size_datagram = (new Datagram((const char*)file_size, 1, 1, (int)strlen((const char*)file_size)));
        size = sendto(socketfd, size_datagram->GetDatagram(), size_datagram->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
    }
    std::cout << "File data sending completed" << std::endl;
    while (true)
    {
        file.stream.read(buffer, BUFFER);
        if (datagram_stack[datagram_counter%stack_size] != NULL)
            delete datagram_stack[datagram_counter%stack_size];
        std::cout << datagram_counter << std::endl;
        datagram_stack[datagram_counter%stack_size] = new Datagram((const char*)buffer, datagram_counter, 2, BUFFER);
	size = sendto(socketfd, datagram_stack[datagram_counter%stack_size]->GetDatagram(), datagram_stack[datagram_counter%stack_size]->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
        //std::cout << size << std::endl;
	if (datagram_stack[datagram_counter%stack_size]->DatagramSize() != size)
        {
            std::cout << "Something went wrong with sending data. " << size << "was sent, but " << datagram_stack[datagram_counter%stack_size]->DatagramSize() << "was expected" << std::endl;
        }
        datagram_counter++;
        if (!is_recieving)
                receive = std::async(std::launch::async, Receive, socketfd, stack_size*0.5*sizeof(int), resend_list, &is_recieving);
            if (resend_list[0] != '\0')
                Resend();
        if (file.stream.peek() == EOF)
            break;
    }
    for (int i = 0; i<20000; i++)
        sendto(socketfd, "<END>", 6, 0, (const struct sockaddr *) &receiver, sizeof(receiver));
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