#include "Sender.h"
#include <unistd.h>
#include <iostream>

Sender::Sender(int _stack_size, char *recv_addr, char *port)
{
    is_recieving = false;
    stack_size = _stack_size;//=STACK; 
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&receiver, 0, sizeof receiver);
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(strtol(port, NULL, 10));
    receiver.sin_addr.s_addr = inet_addr(recv_addr);
    if (socketfd < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    resend_list = new char[int(stack_size*0.5*sizeof(int))];
    datagram_stack = new Datagram*[stack_size];
}
Sender::~Sender()
{
//printf("Freeing memory and closing buffers\n");
    for(int i = 0; i < stack_size; i++)
    {
        datagram_stack[i] = new Datagram();
        delete datagram_stack[i];
    }
    delete[] datagram_stack;
    delete[] resend_list;
    close(socketfd);
    file.stream.close();
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
        receive = std::async(std::launch::async, Receive, socketfd, stack_size*0.5*sizeof(int), resend_list, &ready_to_resend);
    }
}

void Sender::SendFile(char* _file)
{
    file = File((char*)std::filesystem::path(_file).c_str());
    if (!file.stream.is_open())
        return;
    int size;
    char* buffer = new char[BUFFER];
std::cout << "Sending to " << inet_ntoa(receiver.sin_addr) << ":" << htons(receiver.sin_port) << std::endl;

    Datagram* name_datagram = (new Datagram(file.path.filename().c_str(), 0, 0, (int)strlen(file.path.filename().c_str()))); 
    size = sendto(socketfd, name_datagram->GetDatagram(), name_datagram->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
    
    char file_size[sizeof file.size];
    memcpy(file_size, (char*)&file.size, sizeof file.size);
    Datagram* size_datagram = (new Datagram((const char*)file_size, 1, 1, (int)strlen((const char*)file_size)));
    size = sendto(socketfd, size_datagram->GetDatagram(), size_datagram->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
    
    delete name_datagram;
    delete size_datagram;

//std::cout << "File data sending completed" << std::endl;
    while (file.stream.peek() != EOF)
    {
        file.stream.read(buffer, BUFFER);
        if (datagram_stack[datagram_counter%stack_size] != NULL)
            delete datagram_stack[datagram_counter%stack_size];
        datagram_stack[datagram_counter%stack_size] = new Datagram((const char*)buffer, datagram_counter, 2, strlen(buffer));
//printf("Sending data #%d to %s:%d", datagram_counter, inet_ntoa(receiver.sin_addr), htons(receiver.sin_port));
        size = sendto(socketfd, datagram_stack[datagram_counter%stack_size]->GetDatagram(), datagram_stack[datagram_counter%stack_size]->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
//printf("Sent %d bytes to %s:%d\n", size, inet_ntoa(receiver.sin_addr), htons(receiver.sin_port));
        
        datagram_counter++;
        if (datagram_counter % STACK == 0)
            usleep(100);
        StartAsyncRecv();
        if (ready_to_resend)
            Resend();
    }
    for(int i = 0; i < stack_size; i++)
        delete datagram_stack[i];
    for (int i = 0; i<1000; i++)
        sendto(socketfd, "<END>", 6, 0, (const struct sockaddr *) &receiver, sizeof(receiver));
printf("Sending completed. %d datagrams were resent\n", resend_counter);
    return;
}

int Sender::SendDatagram(Datagram* datagram)
{
    if (datagram->data_type != Data)
    {
        std::cout << "Wrong datagram type for a custom send";
        return -1;
    }
    if (datagram_stack[datagram->counter%stack_size] != NULL)
        delete datagram_stack[datagram->counter%stack_size];
    datagram_stack[datagram->counter%stack_size] = datagram;
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
    if (datagram_stack[datagram_counter%stack_size] != NULL)
        delete datagram_stack[datagram_counter%stack_size];
    datagram_stack[datagram_counter%stack_size] = new Datagram(in_data, datagram_counter, datatype, datalen);
    int size = sendto(socketfd, datagram_stack[datagram_counter%stack_size]->GetDatagram(), datagram_stack[datagram_counter%stack_size]->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
//std::cout << "Sent " << size << " bytes to " << inet_ntoa(receiver.sin_addr) << ":" << htons(receiver.sin_port) << "\n";
    datagram_counter++;
    StartAsyncRecv();
    if (ready_to_resend)
        Resend();
    return size;
}

void Sender::ResetCounter()
{
    datagram_counter = 0;
}

void Sender::Resend()
{
//std::cout << "Starting resending data\n";
    ready_to_resend = false;
    is_recieving = false;
    int count = *(int*)(resend_list);
    int lost;
    if (count > (int)(stack_size))
        return;
    for (int i = 1; i < count; i++)
    {
        lost = *(int*)(resend_list+i*sizeof(int));
        if (datagram_stack[lost%stack_size]->counter == lost)
        {
//std::cout << "Resending datagram #" << datagram_stack[lost%stack_size]->counter << "\n";
            resend_counter++;
            sendto(socketfd, datagram_stack[lost%stack_size]->GetDatagram(), datagram_stack[lost%stack_size]->DatagramSize(), 0, (const struct sockaddr *) &receiver, sizeof receiver);
        }
    }
//std::cout << "Resending completed\n";
}
