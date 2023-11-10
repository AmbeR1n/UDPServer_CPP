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
#include <vector>

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
    //// Too Small. Doesnt count header
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
        std::filesystem::path p = std::filesystem::path("download/test.txt");
        int file_size = 0;

        std::ofstream stream;
        stream.open(p, std::ios::out);

        long t1;
        ProgressBar progressbar;
        int current_size = 0;
        int loss = 0;
        int temp_size = 0;
        int size;
        int counter = 0;
        while ((size = recvfrom(sockfd, recv_data, BUFFER, 0, (struct sockaddr *) &sender, &sender_length)) > 0)
        {
            if (strcmp(recv_data, "<END>") == 0)
            {
                sendto(sockfd, "<END>", 6, 0, (const struct sockaddr *)&sender, (socklen_t)sizeof sender);
                break;
            }
            std::unique_ptr<Datagram> datagram(new Datagram(recv_data));
            printf("%d\t%d\t%s:%d\n", datagram->counter, datagram->DatagramSize(), inet_ntoa(sender.sin_addr), htons(sender.sin_port));
            if (datagram->counter != counter)
            {
                sendto(sockfd, "<LOSS>", 7, 0, (const struct sockaddr *)&sender, (socklen_t)sizeof sender);
                int loss_size = datagram->counter-counter;
                while(strcmp(recv_data, "<READY>") != 0)
                {
                    recvfrom(sockfd, recv_data, BUFFER, 0, (struct sockaddr *) &sender, &sender_length);
                    loss_size++;
                }
                loss_size--;
                char* resend_req = new char[sizeof(int) * (loss_size+1)];
                int last = counter + loss_size;
                memcpy(resend_req, &loss_size, sizeof(int));
                memcpy(resend_req + 4, &counter, sizeof(int));
                memcpy(resend_req + 8, &last, sizeof(int));
                // for (int i = 1; i <= loss_size; i++)
                // {
                //     int pack = counter + i;
                //     printf("%d %d\n", i*sizeof(int), pack);
                //     memcpy(resend_req + i * sizeof(int), &(pack), sizeof(int));
                // }
                sendto(sockfd, resend_req, 2 * sizeof loss, 0, (const struct sockaddr *)&sender, (socklen_t)sizeof sender);
                printf("%d Datagrams %d-%d were lost\n", loss_size, counter+1, counter+loss_size);
                delete[] resend_req;
                continue;
            }
            else
                counter++;
            if (datagram->data_type == Filename)
            {
                char* file_name = datagram->GetData();
                //std::cout << "File name received " << file_name << " from "<< inet_ntoa(sender.sin_addr) << "\n";
                p = std::filesystem::path("download/"+std::string(file_name));
            }
            if (datagram->data_type == Filesize)
            { 
                file_size = *(int*)datagram->GetData();
                counter = 2;
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
                    temp_size = 1;
                    //progressbar.PrintLine();
                }
            }
            if (datagram->data_type == Data)
            {
                stream.write(datagram->GetData(), datagram->data_len);
            }
        }
        stream.close();
        progressbar.Update(temp_size, t1);
        printf("%s\t%d\t%d\t%.3f\t%d\n", p.c_str(), current_size, file_size, (1-static_cast<double>(current_size)/file_size)*100, loss);
        progressbar.PrintFinal();
    }
    close(sockfd);
    return 0;
}

