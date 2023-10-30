#include <fstream>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <filesystem>
#include "ProgressBar.h"
#include "Datagram.h"
#include <arpa/inet.h>
#include <iostream>

template<typename T>
long current_time()
{
    return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("App requires 1 args:\tServer Port\nUsing default values:\t5000\n");
        return -1;
    }

    const int BUFFER_SIZE = 25*1024;
    const int S_PORT = std::strtol(argv[1], nullptr, 10);

    
    char recv_data[BUFFER_SIZE];

    struct sockaddr_in reciever;
    struct sockaddr_in sender;
    socklen_t sender_length = 0;
    int sockfd;
    memset(&sender, 0, sizeof(sender));
    memset(&reciever, 0, sizeof(reciever));
    // Filling reciever information
    reciever.sin_family = AF_INET; // IPv4
    reciever.sin_port = htons(S_PORT);
    reciever.sin_addr.s_addr = INADDR_ANY;
    
    // Creating socket file descriptor
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
        
        long t1;
        ProgressBar progressbar;
        int current_size = 0;
        int loss = 0;
        int temp_size = 0;
        int prev_dgram = 1;
        int size;
        while ((size = recvfrom(sockfd, recv_data, BUFFER_SIZE, 0, (struct sockaddr *) &sender, &sender_length)) > 0)
        {
            if (strcmp(recv_data, "<END>") == 0)
            {
                sendto(sockfd, "<END>", 6, 0, (const struct sockaddr *)&sender, (socklen_t)sizeof sender);
                break;
            }
            std::unique_ptr<Datagram> datagram(new Datagram(recv_data));
            //printf("\t%d\t%s:%d\n", datagram->counter, inet_ntoa(sender.sin_addr), htons(sender.sin_port));
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
            if (datagram->data_type == Data)
            {
                current_size += datagram->data_len;
                temp_size += datagram->data_len;
                long t2 = current_time<std::chrono::nanoseconds>();
                if (t2-t1 > 1000000000)
                {
                    progressbar.Update(temp_size, t2);
                    t1 = t2;
                    temp_size = 0;
                    //progressbar.PrintLine();
                }
                if (prev_dgram < datagram->counter)
                {
                    int curr_dgram = datagram->counter;

                    if (curr_dgram - prev_dgram > 1)
                    {
                        int stop = curr_dgram - 1;
                        int start = prev_dgram + 1;
                        loss += stop - start + 1;
                        char resend_req[2 * sizeof loss];
                        memcpy(resend_req, &(start), sizeof start);
                        memcpy(resend_req+sizeof start, &(stop), sizeof stop);
                        if (start == stop)
                            std::cout << "Datagram " << start << " was lost. Sending request to resend lost data\n";
                        else
                            std::cout << "Datagrams from " << start << " to " << stop << " were lost. Sending request to resend lost data\n";
                        size = sendto(sockfd, resend_req, 2 * sizeof loss, 0, (const struct sockaddr *)&sender, (socklen_t)sizeof sender);
                    }
                    prev_dgram = curr_dgram;
                }
                else
                    loss--;
            }
        }
        progressbar.Update(temp_size, t1);
        printf("%s\t%d\t%d\t%.3f\t%d\n", p.c_str(), current_size, file_size, (1-static_cast<double>(current_size)/file_size)*100, loss);
        progressbar.PrintFinal();
    }
    close(sockfd);
    return 0;
}
