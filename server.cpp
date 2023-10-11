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
#include "DatagramParser.h"

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
    const int BUFFER_SIZE = 1500;
    //const in_addr_t ADDRESS = inet_addr(argv[1]);
    const int S_PORT = std::stoi(argv[1]);

    int sockfd;
    char recv_data[BUFFER_SIZE];

    struct sockaddr_in server;
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server, 0, sizeof(server));
    // Filling server information
    server.sin_family = AF_INET; // IPv4
    server.sin_port = htons(S_PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&server,
            sizeof(server)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    //std::cout << "Server started on " << inet_ntoa(server.sin_addr) << ":" << htons(server.sin_port) << std::endl;
    while (true)
    {
        //printf("Waiting for file data...\n");
        int size = recvfrom(sockfd, recv_data, BUFFER_SIZE, 0, NULL, NULL);
        DatagramParser parser = DatagramParser(recv_data);
        std::vector<std::string> header_data = parser.GetHeader();
        long file_size = std::stol(header_data[2].c_str());
        std::string filename = header_data[1];
        //std::cout << "file name: " << filename << " / " << "file size: " << file_size << std::endl;
        //std::filesystem::path p("download/"+filename);
        //ProgressBar progressbar(file_size);
        long current_size = 0;//parser.DataSize();
        //auto t1 = current_time<std::chrono::nanoseconds>();
        //int temp_size = 0;
        // long prev_dgram = std::stol(header_data[0]);
        while ((size = recvfrom(sockfd, recv_data, BUFFER_SIZE, 0, NULL, NULL)) >= 1)
        {
            if (strcmp(recv_data, "<END>") == 0)
                break;
        //     parser = DatagramParser(recv_data);
        //     parser.ExtractHeader();
        //     parser.ExtractTail();
            current_size += size;//parser.DataSize();
        //     temp_size += parser.DataSize();
        //     auto t2 = current_time<std::chrono::nanoseconds>();
        //     if (t2-t1 > 1000000000)
        //     {
        //        progressbar.Update(size, &temp_size);
        //        t1 = t2;
        //        temp_size = 0;
        //     }
        //     else
        //     {
        //         progressbar.Update(size, NULL);
        //     }
        //     header_data = parser.GetHeader();
        //     long dgram_counter = std::stol(header_data[0]);
        //     if (dgram_counter - prev_dgram > 1)
        //     {
        //         printf("                                                          \r%s\t%ld\t%ld\t%ld\n", tail_str.c_str(), dgram_counter, prev_dgram, dgram_counter-prev_dgram);
        //     }
        //     prev_dgram = dgram_counter;
        //     progressbar.PrintLine();
        }   
        std::cout << current_size << "\t" << file_size << "\t" << (1-static_cast<double>(current_size)/file_size)*100 << std::endl;
        break;
        //progressbar.PrintFinal();
        //file.close();
    }
    close(sockfd);
    return 0;
}
