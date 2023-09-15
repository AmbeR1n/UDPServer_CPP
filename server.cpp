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

char** append(char **s, int s_size, char **a, int a_size);
template<typename T>
int64_t current_time()
{
    return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int main(int argc, char *argv[]) 
{
    {
        int default_argc = 3;
        char* default_argv[default_argc] = {argv[0], (char*)"127.0.0.1", (char*)"5010"};
        argv = append(argv, argc, default_argv, default_argc);
        argc = default_argc;
    }

    const char *SEPARATOR = "<SEP>";
    const int BUFFER_SIZE = 1024*60;
    const in_addr_t ADDRESS = inet_addr(argv[1]);
    const int S_PORT = strtoull(argv[2], NULL, 0);

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

    int file_size = 0;
    sscanf(filesize, "%d", &file_size);
    std::ofstream file;
    std::filesystem::path p("download/"+filename);
    file.open (p, std::fstream::trunc);
    int current_size = 0;
    char *progress_bar[] = { (char *)"\u002D", (char *)"\u002D", (char *)"\u002D", (char *)"\u002D", (char *)"\u002D",
                               (char *)"\u002D", (char *)"\u002D", (char *)"\u002D", (char *)"\u002D", (char *)"\u002D",
                               (char *)"\u002D", (char *)"\u002D", (char *)"\u002D", (char *)"\u002D", (char *)"\u002D",
                               (char *)"\u002D", (char *)"\u002D", (char *)"\u002D", (char *)"\u002D", (char *)"\u002D" };
    auto start = current_time<std::chrono::seconds>();
    auto p1 = current_time<std::chrono::nanoseconds>();
    while (int size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL))
        {
        file << buffer;
        double speed = size/(p1-current_time<std::chrono::nanoseconds>())*1000000000/1024;
        p1 = current_time<std::chrono::nanoseconds>();
        current_size += size;
        auto duration = current_time<std::chrono::seconds>()-start;
        double progress = (double)current_size/file_size*100;
        progress_bar[(int)progress / 5] = (char *)"\u25A0";
        std::cout << duration / 60 << ":" << duration % 60 << progress << progress_bar << speed << "B/s" << "\n" << std::flush;
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

