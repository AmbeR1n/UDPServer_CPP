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
    const char *SEPARATOR = "<SEP>";
    const int BUFFER_SIZE = 1024*10;
    const in_addr_t ADDRESS = inet_addr(argv[1]);
    const int S_PORT = strtol(argv[2], NULL, 0);

    int sockfd;
    char buffer[BUFFER_SIZE];
    const char *response = "<OK>";
    struct sockaddr_in server;
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    int server_len = sizeof(server);
    memset(&server, 0, server_len);
       
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

    int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, NULL, NULL);

    std::string fileinfo = buffer; 
    const char *filesize = fileinfo.substr(0, fileinfo.find(SEPARATOR)).c_str();
    std::string filename = fileinfo.substr(fileinfo.find(SEPARATOR)+5);
    std::cout << "file name: " << filename << " / " << "file size: " << filesize << std::endl;

    uint64_t file_size = 0;
    sscanf(filesize, "%llu", &file_size);
    std::ofstream file;
    std::filesystem::path p("download/"+filename);
    file.open (p, std::fstream::trunc);
    ProgressBar progressbar(file_size);
    uint64_t current_size = 0;
    while (int size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL))
    {
        //file << buffer;
        current_size += size;
        progressbar.Update(current_size);
        progressbar.PrintLine();
    }
    file.close();

    close(sockfd);
    return 0;
}
