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

template<typename T>
int64_t current_time()
{
    return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int main(int argc, char *argv[]) 
{
    if (argc != 3)
    {
        std::cout << "App requires 2 args:\tServer IP\tServer Port\nUsing default values:\t127.0.0.1\t5010\n";
        const int default_argc = 3;
        char* default_argv[default_argc] = {argv[0], (char*)"127.0.0.1", (char*)"5010"};
        argv = default_argv;
        argc = default_argc;
    }
    const char SEPARATOR[] = "<SEP>";
    const int BUFFER_SIZE = 1024 * 12;
    //const in_addr_t ADDRESS = inet_addr(argv[1]);
    const int S_PORT = strtol(argv[2], NULL, 10);

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
    std::cout << "Server started on " << inet_ntoa(server.sin_addr) << ":" << htons(server.sin_port) << std::endl;
    while (true)
    {
        printf("Waiting for file data...\n");
        int n = recvfrom(sockfd, (char *)recv_data, BUFFER_SIZE, 0, NULL, NULL);
        if (n == -1)
            return -1;
        std::string fileinfo = recv_data; 
        const char *filesize = fileinfo.substr(0, fileinfo.find(SEPARATOR)).c_str();
        std::string filename = fileinfo.substr(fileinfo.find(SEPARATOR)+5);
        std::cout << "file name: " << filename << " / " << "file size: " << filesize << std::endl;

        uint64_t file_size = 0;
        sscanf(filesize, "%lu", &file_size);
        std::ofstream file;

        std::filesystem::path p("download/"+filename);
        file.open (p, std::fstream::trunc);
        ProgressBar progressbar(file_size);
        uint64_t current_size = 0;
        auto t1 = current_time<std::chrono::nanoseconds>();
        int temp_size = 0;
        long prev_dgram = 0;
        while (int size = recvfrom(sockfd, recv_data, BUFFER_SIZE, 0, NULL, NULL))
        {
            // file << recv_data;
            current_size += size;   
            temp_size += size;
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
            char tail[30];
            strncpy(tail, recv_data+BUFFER_SIZE-30, 30);
            std::string tail_str = std::string(tail);
            long dgram_counter = strtol(tail_str.substr(5, 25).c_str(), NULL, 10);
            if (dgram_counter - prev_dgram > 1)
            {
                //printf("                                                          \r%s\t%ld\t%ld\t%ld\n", tail_str.c_str(), dgram_counter, prev_dgram, dgram_counter-prev_dgram);
            }
            prev_dgram = dgram_counter;
            progressbar.PrintLine();
        }
        printf("\n%lu\t%lu\t%.1f\n", file_size, current_size, static_cast<double>(file_size - current_size)/file_size*100);
        //progressbar.PrintFinal();
        file.close();
    }
    close(sockfd);
    return 0;
}
