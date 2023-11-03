#include "Sender.h"
#include <unistd.h>
#include <iostream>

Sender::Sender(char *recv_addr, char *port)
{
    is_recieving = false;
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&receiver, 0, sizeof receiver);
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(strtol(port, NULL, 10));
    receiver.sin_addr.s_addr = inet_addr(recv_addr);
    if (socketfd < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    resend_list = new char[STACK*sizeof(int)];
    datagram_stack = new Datagram*[STACK];
}
Sender::~Sender()
{
//printf("Freeing memory and closing buffers\n");
    for(int i = 0; i < STACK; i++)
    {
        datagram_stack[i] = new Datagram();
        delete datagram_stack[i];
    }
    delete[] datagram_stack;
    delete[] resend_list;
    close(socketfd);
    
//printf("Done.\n");
}

void Sender::Receive(int socket, int buff_size, char* buffer, bool* flag_resend)
{
//std::cout << "waiting for request for resending lost data\n";
    recvfrom(socket, buffer, buff_size, 0, NULL, NULL);
    *flag_resend = true;
//std::cout << "Received request for resending lost data\n";
}

void Sender::StartAsyncRecv()
{
    if (!is_recieving)
    {
        is_recieving = true;
        receive = std::async(std::launch::async, Receive, socketfd, STACK*sizeof(int), resend_list, &ready_to_resend);
    }
}

void Sender::SendEnd()
{
    for (int i = 0; i<1000; i++)
        sendto(socketfd, "<END>", 6, 0, (const struct sockaddr *) &receiver, sizeof(receiver));
}

void Sender::SendFile(char *_file)
{
    file = File((char*)std::filesystem::path(_file).c_str());
    if (!file.stream.is_open())
        return;
    char* buffer = new char[BUFFER+1];
    buffer[BUFFER] = '\0';
//std::cout << "Sending to " << inet_ntoa(receiver.sin_addr) << ":" << htons(receiver.sin_port) << std::endl;

    datagram_stack[0] = (new Datagram(file.path.filename().c_str(), 0, (int)strlen(file.path.filename().c_str()))); 
    sendto(socketfd, datagram_stack[0]->GetDatagram(), datagram_stack[0]->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
    char file_size[sizeof file.size];
    memcpy(file_size, (char*)&file.size, sizeof file.size);
    datagram_stack[1] = (new Datagram((const char*)file_size, 1, (int)strlen((const char*)file_size)));
    sendto(socketfd, datagram_stack[1]->GetDatagram(), datagram_stack[1]->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
//std::cout << "File data sending completed" << std::endl;
    while (file.stream.peek() != EOF)
    {
        file.stream.read(buffer, BUFFER);
        //if (datagram_stack[datagram_counter%STACK] != NULL)
        //    delete datagram_stack[datagram_counter%STACK];
        datagram_stack[Datagram::global_counter%STACK + 1] = new Datagram((const char*)buffer, 2, strlen(buffer));
//printf("Sending data #%d to %s:%d", datagram_counter, inet_ntoa(receiver.sin_addr), htons(receiver.sin_port));
        sendto(socketfd, datagram_stack[Datagram::global_counter%STACK]->GetDatagram(), datagram_stack[Datagram::global_counter%STACK]->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
//printf("Sent %d bytes to %s:%d\n", size, inet_ntoa(receiver.sin_addr), htons(receiver.sin_port));
        StartAsyncRecv();
        if (ready_to_resend)
            Resend();
    }
    //for(int i = 0; i < STACK; i++)
    //    delete datagram_stack[i];
    SendEnd();
    file.stream.close();
    delete[] buffer;
//printf("Sending completed. %d datagrams were resent\n", resend_counter);
    return;
}

int Sender::SendDatagram(Datagram* datagram)
{
    if (datagram->data_type != Data)
    {
        std::cout << "Wrong datagram type for a custom send";
        return -1;
    }
    //if (datagram_stack[datagram->counter%STACK] != NULL)
    //    delete datagram_stack[datagram->counter%STACK];
    datagram_stack[datagram->counter%STACK] = datagram;
    int size = sendto(socketfd, datagram->GetDatagram(), datagram->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
//std::cout << "Sent " << size << " bytes to " << inet_ntoa(receiver.sin_addr) << ":" << htons(receiver.sin_port) << "\n";
    StartAsyncRecv();
    if (ready_to_resend)
        Resend();
    return size;
}

int Sender::SendDatagram(const char *in_data, int datatype, int datalen)
{
    if (datatype != Data)
    {
        std::cout << "Wrong datagram type for a custom send";
        return -1;
    }
    //if (datagram_stack[datagram_counter%STACK] != NULL)
    //    delete datagram_stack[datagram_counter%STACK];
    datagram_stack[Datagram::global_counter%STACK+1] = new Datagram(in_data, datatype, datalen);
    int size = sendto(socketfd, datagram_stack[Datagram::global_counter%STACK]->GetDatagram(), datagram_stack[Datagram::global_counter%STACK]->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
//std::cout << "Sent " << size << " bytes to " << inet_ntoa(receiver.sin_addr) << ":" << htons(receiver.sin_port) << "\n";
    StartAsyncRecv();
    if (ready_to_resend)
        Resend();
    return size;
}

void Sender::Resend()
{
    ready_to_resend = false;
    is_recieving = false;
    int count;
    memcpy(&count, resend_list, sizeof count);
std::cout << "Starting resending " << count << " datagrams.\n";
    int lost;
    if (count > STACK)
        return;
    for (int i = 1; i < count; i++)
    {
        memcpy(&lost, resend_list+i*sizeof(int), sizeof lost);
std::cout << "looking for datagram #" << lost << "\n";
        if (datagram_stack[lost%STACK]->counter == lost)
        {
            resend_counter++;
            sendto(socketfd, datagram_stack[lost%STACK]->GetDatagram(), datagram_stack[lost%STACK]->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
        }
    }
//std::cout << "Resending completed\n";
}
