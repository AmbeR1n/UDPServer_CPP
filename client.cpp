#include <bits/stdc++.h>
#include <arpa/inet.h>
//#include <iostream>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <fstream>
//#include <filesystem>
//#include <stdlib.h>
//#include <unistd.h>

char** append(char **s, int s_size, char **a, int a_size);

int main(int argc, char *argv[]) 
{
    {
        int default_argc = 5;
        char* default_argv[default_argc] = {argv[0], (char*)"train.txt", (char*)"0.0.0.0", (char*)"5009", (char*)"5007"};
        argv = append(argv, argc, default_argv, default_argc);
        argc = default_argc;
    }
    const int BUFFER_SIZE = 1024 * 16;
    const char *SEPARATOR = "<SEP>";
    const char *FILE_NAME = argv[1];
    const in_addr_t ADDRESS = inet_addr(argv[2]);
    const int S_PORT = strtoull(argv[3], NULL, 0);
    const int C_PORT = strtoull(argv[4], NULL, 0);

    int sockfd;
    char response[100];
    struct sockaddr_in serversocket;
    char *data = new char[BUFFER_SIZE];

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
   
    memset(&serversocket, 0, sizeof(serversocket));
    // Filling server information
    serversocket.sin_family = AF_INET;
    serversocket.sin_port = htons(S_PORT);
    inet_pton(AF_INET, "localhost", &serversocket.sin_addr);
       
    // Collecting file info
    std::filesystem::path filepath {FILE_NAME};
    int filesize = std::filesystem::file_size(filepath);
    std::string fileinfo = std::to_string(filesize) + SEPARATOR + FILE_NAME;

    // Sending file info and waiting for response
    int n = sendto(sockfd, fileinfo.c_str(), fileinfo.size(), 
        0, (const struct sockaddr *) &serversocket, sizeof(serversocket));
    std::cout << "file info sent to " << inet_ntoa(serversocket.sin_addr) << ":" << htons(serversocket.sin_port) << std::endl;
    //socklen_t len;
    //int n = recvfrom(sockfd, (char *)response, 4, 0, NULL, NULL);
    //response[n] = '\0';
    printf("sent: %i bytes from client to %s:%i\n", n,
		       inet_ntoa(serversocket.sin_addr), htons(serversocket.sin_port));

    // Sending file
    std::ifstream file;
    file.open(filepath, std::ios::in);
    int current_size = 0;
    while (current_size < filesize)
    {
        file.read(data, BUFFER_SIZE-1);
        int size = sendto(sockfd, (const char *)data, BUFFER_SIZE,
            MSG_CONFIRM, (const struct sockaddr *) &serversocket, sizeof(serversocket));
        current_size += size;
        std::cout<<"file data sent. "<< (double)current_size/filesize*100 << "%\r" <<std::flush;
        //n = recvfrom(sockfd, (char *)response, 4, 0, NULL, NULL);
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