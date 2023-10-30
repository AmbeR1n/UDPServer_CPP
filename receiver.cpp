#include <fstream>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <filesystem>
#include "ProgressBar.h"
#include "Datagram.h"
#include <arpa/inet.h>
#include <iostream>
#include <fstream>

template<typename T>
long current_time()
{
    return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("receiver server_port\n");
        return -1;
    }

    const int S_PORT = std::strtol(argv[1], nullptr, 10);
    
    char recv_data[BUFFER];

    struct sockaddr_in reciever;
    struct sockaddr_in sender;
    socklen_t sender_length = 0;
    
    memset(&sender, 0, sizeof(sender));
    memset(&reciever, 0, sizeof(reciever));
    // Filling reciever information
    reciever.sin_family = AF_INET; // IPv4
    reciever.sin_port = htons(S_PORT);
    reciever.sin_addr.s_addr = INADDR_ANY;
    
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    if (bind(sockfd, (const struct sockaddr *)&reciever, sizeof(reciever)) < 0 )
    {
        perror("socket binding failed");
        exit(EXIT_FAILURE);
    }

std::cout << "Server started on " << inet_ntoa(reciever.sin_addr) << ":" << htons(reciever.sin_port) << std::endl;
    //while (true)
    {
        std::filesystem::path p;
        int file_size = 0;

        const char* file_name = "download/test.txt";
        std::ofstream stream;
        stream.open(file_name, std::ios::out);
        bool status[STACK];
        for (int i = 0; i < STACK; i++)
            status[i] = false;
        long t1;
        ProgressBar progressbar;
        int current_size = 0;
        int loss = 0;
        int temp_size = 0;
        int prev_dgram = 1;
        int size;
        while ((size = recvfrom(sockfd, recv_data, BUFFER, 0, (struct sockaddr *) &sender, &sender_length)) > 0)
        {
            if (strcmp(recv_data, "<END>") == 0)
            {
                sendto(sockfd, "<END>", 6, 0, (const struct sockaddr *)&sender, (socklen_t)sizeof sender);
                break;
            }
            std::unique_ptr<Datagram> datagram(new Datagram(recv_data));
//printf("%d\t%d\t%s:%d\n", datagram->counter, datagram->DatagramSize(), inet_ntoa(sender.sin_addr), htons(sender.sin_port));
            
            if (datagram->data_type == Filename)
            {
                char* file_name = datagram->GetData();
//std::cout << "File name received " << file_name << " from "<< inet_ntoa(sender.sin_addr) << "\n";
                p = std::filesystem::path("download/"+std::string(file_name));
            }
            if (datagram->data_type == Filesize)
            { 
                file_size = *(int*)datagram->GetData();
//std::cout << "File size received " << file_size << " from "<< inet_ntoa(sender.sin_addr) << "\n";
                t1 = current_time<std::chrono::nanoseconds>();
                progressbar = ProgressBar(file_size, t1);
            }
            if (datagram->data_type == FileData)
            {
                current_size += datagram->data_len;
                temp_size += datagram->data_len;
                long t2 = current_time<std::chrono::nanoseconds>();
                if (t2-t1 > 1000000000)
                {
                    progressbar.Update(temp_size, t2);
                    t1 = t2;
                    temp_size = 0;
                }

                if(datagram->counter%STACK == 0)
                {
                    int mult = datagram->counter / STACK;
                    int c = 0;
                    char resend_req[(STACK+1) * sizeof loss];
                    for (int i = 0; i < STACK; i++)
                    {   
                        if (!status[i])
                        {
                            c++;
                            int value = i + mult*STACK;
                            memcpy(resend_req + (i+1)*sizeof value, &(value), sizeof value);
                        }
                    }
//std::cout << c << " Datagrams were lost. Sending request for resend";
                    memcpy(resend_req, &(c), sizeof c);
                    if (c > 0)
                        size = sendto(sockfd, resend_req, STACK * sizeof loss, 0, (const struct sockaddr *)&sender, (socklen_t)sizeof sender);
                }
                if (prev_dgram < datagram->counter)
                {
                    int curr_dgram = datagram->counter;
                    loss += curr_dgram - prev_dgram - 1;
                    prev_dgram = curr_dgram;
                }
                else
                    loss--;
            }
            if (datagram->data_type == Data)
            {
                stream.write(datagram->GetData(), datagram->data_len);
            }
            status[datagram->counter%STACK] = true;
        }
        stream.close();
        progressbar.Update(temp_size, t1);
        printf("%s\t%d\t%d\t%.3f\t%d\n", p.c_str(), current_size, file_size, (1-static_cast<double>(current_size)/file_size)*100, loss);
        progressbar.PrintFinal();
    }
    close(sockfd);
    return 0;
}
