#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include "ProgressBar.h"
#include "DatagramParser.h"

template<typename T>
long current_time()
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

    const int BUFFER_SIZE = 1024 * 12; // 12kB
    //const char SEPARATOR[] = "<SEP>";
    const char *FILE_NAME = "train.txt";
    const char *ADDRESS = argv[1];
    const int S_PORT = strtol(argv[2], NULL, 10);

    int sockfd;
    //char response[100];
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
    long file_size = std::filesystem::file_size(filepath) * 5;

    // Sending file
    std::ifstream file;
    file.open(filepath, std::ios::in);
    char data[BUFFER_SIZE];
    file.read(data, BUFFER_SIZE-128);
    ProgressBar progressbar = ProgressBar(file_size);
    DatagramParser parser = DatagramParser(std::string(data));
    long current_size = 0;
    int temp_size = 0;
    printf("About to start sending %lu bytes of data(%.1fGB)...", file_size, (double)file_size/1024/1024/1024);
    getchar();
    auto t1 = current_time<std::chrono::nanoseconds>();
    long counter = 1;
    while (current_size < file_size)
    {
        parser = DatagramParser(std::string(data));
        parser.BuildHeader(std::vector<std::string>{std::to_string(counter), std::string(FILE_NAME), std::to_string(file_size)});
        counter++;
        int size = sendto(sockfd, parser.BuildDatagram().c_str(), BUFFER_SIZE, 0, (const struct sockaddr *) &server, sizeof(server));
        auto t2 = current_time<std::chrono::nanoseconds>();
        current_size += size;
        temp_size += size;
        
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
        progressbar.PrintLine();
    }
    printf("\n");
    progressbar.PrintFinal();
    file.close();    
    close(sockfd);
    return 0;
}