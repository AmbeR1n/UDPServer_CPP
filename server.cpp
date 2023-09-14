#include <bits/stdc++.h>
#include <arpa/inet.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <filesystem>

char** append(char **s, int s_size, char **a, int a_size);

int main(int argc, char *argv[]) 
{
    {
        int default_argc = 4;
        char* default_argv[default_argc] = {argv[0], (char*)"0.0.0.0", (char*)"5009", (char*)"5007"};
        argv = append(argv, argc, default_argv, default_argc);
        argc = default_argc;
    }

    const char *SEPARATOR = "<SEP>";
    const int BUFFER_SIZE = 1024*16;
    const in_addr_t ADDRESS = inet_addr(argv[1]);
    const int S_PORT = strtoull(argv[2], NULL, 0);
    const int C_PORT = strtoull(argv[3], NULL, 0);

    int sockfd;
    char buffer[BUFFER_SIZE];
    const char *response = "<OK>";
    struct sockaddr_in serversocket, clientsocket;
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
       
    memset(&serversocket, 0, sizeof(serversocket));
    memset(&clientsocket, 0, sizeof(clientsocket));
       
    // Filling server information
    serversocket.sin_family = AF_INET; // IPv4
    serversocket.sin_port = htons(S_PORT);
    serversocket.sin_addr.s_addr = INADDR_ANY;
    
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&serversocket, 
            sizeof(serversocket)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Server started on " << inet_ntoa(serversocket.sin_addr) << ":" << htons(serversocket.sin_port) << std::endl;
    socklen_t len;
    int n;
   

    n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, ( struct sockaddr *) &clientsocket, &len);
    buffer[n] = '\0';
    clientsocket.sin_port = htons(C_PORT);
    printf("received: %i bytes from client %s:%i\n", n,
		       inet_ntoa(clientsocket.sin_addr), htons(clientsocket.sin_port));
    //n = sendto(sockfd, response, 4, 0, (const struct sockaddr *) &clientsocket, sizeof(clientsocket));
    //printf("response sent with %i bytes\n", n);

    std::string fileinfo = buffer; 
    const char *filesize = fileinfo.substr(0, fileinfo.find(SEPARATOR)).c_str();
    std::string filename = fileinfo.substr(fileinfo.find(SEPARATOR)+5);
    std::cout << "file name: " << filename << " / " << "file size: " << filesize << std::endl;

    int file_size = 0;
    sscanf(filesize, "%d", file_size);
    std::ofstream file;
    std::filesystem::path p("download/"+filename);
    file.open (p, std::fstream::trunc);
    int current_size = 0;
    while (n = recvfrom(sockfd, buffer, BUFFER_SIZE, 
                0, ( struct sockaddr *) &clientsocket, &len))
        {
        file << buffer;
        current_size += n;
        std::cout<<"received "<<(double)current_size/file_size*100 << "%\r" <<std::flush;
        }
    file.close();
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