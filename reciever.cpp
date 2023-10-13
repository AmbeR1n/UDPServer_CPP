#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <filesystem>
#include "ProgressBar.h"
#include "Datagram.h"

template<typename T>
int64_t current_time()
{
    return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "App requires 1 args:\tServer Port\nUsing default values:\t5000\n";
        return -1;
    }
    //const char SEPARATOR[] = "<SEP>";
    const int BUFFER_SIZE = 12*1024;
    //const in_addr_t ADDRESS = inet_addr(argv[1]);
    const int S_PORT = std::stoi(argv[1]);

    
    char recv_data[BUFFER_SIZE];

    struct sockaddr_in reciever;
    struct sockaddr_in sender;
    socklen_t client_length = 0;
    int sockfd;

    memset(&sender, 0, sizeof(sender));
    memset(&reciever, 0, sizeof(reciever));
    // Filling reciever information
    reciever.sin_family = AF_INET; // IPv4
    reciever.sin_port = htons(S_PORT);
    reciever.sin_addr.s_addr = INADDR_ANY;
    
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) > -1 ) 
    {
        // Bind the socket with the reciever address
        if ( bind(sockfd, (const struct sockaddr *)&reciever, sizeof(reciever)) < 0 )
            exit(EXIT_FAILURE);
    }
    else exit(EXIT_FAILURE);

    
    //std::cout << "Server started on " << inet_ntoa(reciever.sin_addr) << ":" << htons(reciever.sin_port) << std::endl;
    while (true)
    {
        char* file_name = (char*)"template.txt";
        int file_size = 0;
        
        int size = recvfrom(sockfd, recv_data, BUFFER_SIZE, 0, (struct sockaddr *) &sender, &client_length);
        Datagram datagram = Datagram(recv_data);
        if (datagram.data_type == Filename)
            file_name = datagram.data;

        size = recvfrom(sockfd, recv_data, BUFFER_SIZE, 0, (struct sockaddr *) &sender, &client_length);
        if (datagram.data_type == Filesize) 
            file_size = *(int*)datagram.data;

        std::filesystem::path p("download/"+std::string(file_name));
        ProgressBar progressbar(file_size);

        auto t1 = current_time<std::chrono::nanoseconds>();
        int current_size = 0;
        int loss = 0;
        int temp_size = 0;
        int prev_dgram = datagram.counter;
        while ((size = recvfrom(sockfd, recv_data, BUFFER_SIZE, 0, (struct sockaddr *) &sender, &client_length)) >= 1)
        {
            if (strcmp(recv_data, "<END>") == 0)
                break;
            datagram = Datagram(recv_data);
            if (datagram.data_type != Data)
                continue;
            current_size += datagram.data_len;
            temp_size += datagram.data_len;
            auto t2 = current_time<std::chrono::nanoseconds>();
            if (t2-t1 > 1000000000)
            {
               progressbar.Update(size, &temp_size);
               t1 = t2;
               temp_size = 0;
            }
            else
            {
                progressbar.Update(size, NULL);
            }
            int dgram_counter = datagram.counter;

            if (dgram_counter - prev_dgram > 1)
                loss += dgram_counter - prev_dgram - 1;
            prev_dgram = dgram_counter;
        } 
        printf("%d\t%d\t%.3f\t%d\t%d\n", current_size, file_size, (1-static_cast<double>(current_size)/file_size)*100, loss, prev_dgram);
        progressbar.PrintFinal();
        
    }
    close(sockfd);
    return 0;
}
