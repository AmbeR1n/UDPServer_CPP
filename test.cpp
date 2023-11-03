#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cstring>
#include "Datagram.h"

int main(int argc, char** argv)
{
    if(false)
    {
        const char* file_i = "upload/train.txt";
        const char* file_o = "upload/train_o.txt";
        char line[100];
        line[99] = '\0';
        std::ofstream ostream;
        std::ifstream istream;
        istream.open(file_i, std::ios_base::in);
        ostream.open(file_o, std::ios_base::out);
        int counter = 0;
        while(true)
        {
            istream.read(line, 100-1);
            
            if (counter == 100)
                break;
            std::string new_line = std::to_string(counter)+line+"\n";
            printf("%s\n", new_line.c_str());   
            ostream.write(new_line.c_str(), new_line.size());
            counter++;
        }
        istream.close();
        ostream.close();
        std::cout << "\n";
    }
    if(false)
    {
        char array[20];
        memset(array, 'a', 20);
        array[20] = '\0';
        std::cout << array << std::endl;
        int a = 15;
        int b = 20;
        int c = 30;
        memcpy(array, &a, sizeof a);
        memcpy(array + sizeof a, &b, sizeof b);
        memcpy(array + sizeof a + sizeof b, &c, sizeof c);
        std::cout << &array[sizeof a + sizeof b + sizeof c] << std::endl;
    }
    if(true)
    {
        Datagram** array = new Datagram*[10];
        for (int i = 0; i<10;i++)
        {
            array[i%10] = new Datagram("abc", 3, strlen("abc"));
            std::cout << Datagram::global_counter << " " << i << " " << array[i%10]->counter << std::endl;
        }

    }
}  