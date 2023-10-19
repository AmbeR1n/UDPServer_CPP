#include <arpa/inet.h>
#include <fstream>
#include <memory>
#include <chrono>
#include <filesystem>
#include <unistd.h>
#include <cstring>
#include "ProgressBar.h"
#include "Datagram.h"

template<typename T>
int current_time()
{
    return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int main(int argc, char *argv[]) 
{
    if (argc != 3)
    {
        printf("App requires 2 args:\tServer IP\tServer Port\nFor example:\t127.0.0.1\t5000\n");
        return -1;
    }
    const int BUFFER_SIZE = 1024*63; // 12kB
    const char *FILE_NAME = "train.txt";
    const char *ADDRESS = argv[1];
    const int S_PORT = strtol(argv[2], NULL, 10);
    struct sockaddr_in reciever;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // Creating socket file descriptor
    if (sockfd < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Filling reciever information
    memset(&reciever, 0, sizeof(reciever));
    reciever.sin_family = AF_INET;
    reciever.sin_port = htons(S_PORT);
    reciever.sin_addr.s_addr = inet_addr(ADDRESS);

    // Collecting file info
    std::filesystem::path filepath("upload/"+std::string(FILE_NAME));
    long file_size = std::filesystem::file_size(filepath) * 10;
    
    char file_data[sizeof file_size];
    memcpy(file_data, (char*)&file_size, sizeof file_size);
    int size;
    {
        std::unique_ptr<Datagram> name_datagram(new Datagram(FILE_NAME, 0, 0, strlen(FILE_NAME)));
        size = sendto(sockfd, name_datagram->GetDatagram(), name_datagram->DatagramSize(), 0, (const struct sockaddr *) &reciever, sizeof(reciever));
        std::unique_ptr<Datagram> size_datagram(new Datagram((const char*)file_data, 1, 1, strlen(file_data)));
        size = sendto(sockfd, size_datagram->GetDatagram(), size_datagram->DatagramSize(), 0, (const struct sockaddr *) &reciever, sizeof(reciever));
    }
    // Sending file
    std::ifstream file;
    file.open(filepath, std::ios::in);
    char data[BUFFER_SIZE];
    file.read(data, BUFFER_SIZE);
    
    long current_size = 0;
    int temp_size = 0;
    Datagram* datagram = new Datagram((const char*)data, 2, 2, strlen(data));
    printf("About to start sending %ld bytes of data(%.1fGB)...", file_size, (double)file_size/1024/1024/1024);
    getchar();
    int t1 = current_time<std::chrono::nanoseconds>();
    ProgressBar progressbar = ProgressBar(file_size, t1);
    while (current_size < file_size)
    {
        size = sendto(sockfd, datagram->GetDatagram(), datagram->DatagramSize(), 0, (const struct sockaddr *) &reciever, sizeof(reciever));
        if (file.eof())
        {
            file.clear();
            file.seekg(0);
        }
        file.read(data, BUFFER_SIZE);
        datagram->counter++;
        datagram->SetData(data, strlen(data));

        current_size += size;
        int t2 = current_time<std::chrono::nanoseconds>();
        temp_size += size;
        if (t2-t1 > 1000000000)
        {
            progressbar.Update(temp_size, t2);
            t1 = t2;
            temp_size = 0;
            progressbar.PrintLine();
        }
    }
    
    progressbar.Update(temp_size, t1);
    progressbar.PrintFinal();
    for (int i = 0; i<20000; i++)
        size = sendto(sockfd, "<END>", BUFFER_SIZE, 0, (const struct sockaddr *) &reciever, sizeof(reciever));
    close(sockfd);
    file.close();
    delete datagram;
    return 0;
}
