#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <string.h>
#include "ProgressBar.h"

char** append(char **s, int s_size, char **a, int a_size);
std::string convert_str(char **s, int size);
template<typename T>
int64_t current_time()
{
    return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int main(int argc, char *argv[]) 
{
    {
        const int default_argc = 4;
        char* default_argv[default_argc] = {argv[0], (char*)"train.txt", (char*)"127.0.0.1", (char*)"5010"};
        argv = append(argv, argc, default_argv, default_argc);
        argc = default_argc;
    }
    const int BUFFER_SIZE = 1024 * 60;
    const char *SEPARATOR = "<SEP>";
    const char *FILE_NAME = argv[1];
    const char *ADDRESS = argv[2];
    const int S_PORT = strtoull(argv[3], NULL, 0);

    int sockfd;
    char response[100];
    struct sockaddr_in server;
    char *data = new char[BUFFER_SIZE];

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
    int file_size = std::filesystem::file_size(filepath);
    std::string fileinfo = std::to_string(file_size) + SEPARATOR + FILE_NAME;

    // Sending file info
    int n = sendto(sockfd, fileinfo.c_str(), fileinfo.size(), 
        0, (const struct sockaddr *) &server, sizeof(server));
    std::cout << "file info sent to " << inet_ntoa(server.sin_addr) << ":" << htons(server.sin_port) << std::endl;

    // Sending file
    std::ifstream file;
    file.open(filepath, std::ios::in);
    ProgressBar progressbar;
    int current_size = 0;
    char *progress_bar[] = {(char *)"-", (char *)"-", (char *)"-", (char *)"-", (char *)"-", (char *)"-", (char *)"-", (char *)"-", (char *)"-", (char *)"-", 
                            (char *)"-", (char *)"-", (char *)"-", (char *)"-", (char *)"-", (char *)"-", (char *)"-", (char *)"-", (char *)"-", (char *)"-", };
    auto start = current_time<std::chrono::seconds>();
    auto p1 = current_time<std::chrono::nanoseconds>();
    while (current_size < file_size)
    {
        file.read(data, BUFFER_SIZE-1);
        int size = sendto(sockfd, (const char *)data, BUFFER_SIZE, 0, (const struct sockaddr *) &server, sizeof(server));
        double speed = size/(p1-current_time<std::chrono::nanoseconds>())*1000000000/1024;
        p1 = current_time<std::chrono::nanoseconds>();
        current_size += size;
        auto duration = current_time<std::chrono::seconds>()-start;
        double progress = (double)current_size/file_size*100;
        progress_bar[(int)progress / 5] = "+";
        std::string str_progress = convert_str(progress_bar, 20);
        std::cout/* << std::setw(2) << std::setfill('0') << duration / 60 << ":" << std::setw(2) << std::setfill('0') << duration % 60*/ << progress << str_progress/* << speed << "B/s" */<< "\r" << std::flush;
    }
    file.close();
    
    close(sockfd);
    return 0;
}

char** append(char **s, int s_size, char **a, int a_size)
{
    char **output = new char*[a_size+1];
    for (int i = 0; i < s_size; i++)
        output[i] = s[i];
    for (int i = s_size; i < a_size; i++)
        output[i] = a[i];
    return output;
}

std::string convert_str(char **s, int size)
{
    std::string output;
    for (int i = 0; i < size; i++)
        output.append((const char *)s[i]);
    return output;
}
