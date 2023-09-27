#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <string.h>
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

    const int BUFFER_SIZE = 1024 * 10;
    const char *SEPARATOR = "<SEP>";
    const char *FILE_NAME = "train.txt";
    const char *ADDRESS = argv[1];
    const int S_PORT = strtol(argv[2], NULL, 0);

    int sockfd;
    char response[100];
    struct sockaddr_in server;
    

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Filling server information
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(S_PORT);
    inet_pton(AF_INET, ADDRESS, &server.sin_addr);

    // Collecting file info
    std::filesystem::path filepath("upload/"+std::string(FILE_NAME));
    uint64_t file_size = std::filesystem::file_size(filepath) * 4;

    // Sendind file info
    std::string fileinfo = std::to_string(file_size) + SEPARATOR + FILE_NAME;
    int n = sendto(sockfd, fileinfo.c_str(), fileinfo.size(),
        0, (const struct sockaddr *) &server, sizeof(server));
    std::cout << "file info sent to " << inet_ntoa(server.sin_addr) << ":" << htons(server.sin_port) << std::endl;

    // Sending file
    std::ifstream file;
    file.open(filepath, std::ios::in);
    char *data = new char[BUFFER_SIZE];
    file.read(data, BUFFER_SIZE);
    ProgressBar progressbar = ProgressBar(file_size);
    uint64_t current_size;
    printf("About to start sending %llu bytes of data(%lluGB)...", file_size, file_size/1024/1024/1024);
    getchar();
    while (current_size < file_size)
    {
        int size = sendto(sockfd, (const char *)data, BUFFER_SIZE, 0, (const struct sockaddr *) &server, sizeof(server));
        current_size += size;
        progressbar.Update(current_size);
        progressbar.PrintLine();
    }
    printf("\n");
    file.close();    
    close(sockfd);
    return 0;
}