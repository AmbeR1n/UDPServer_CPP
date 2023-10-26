#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cstring>

int main(int argc, char** argv)
{
#ifdef false
    const char* file_i = "upload/train.txt";
    const char* file_o = "upload/train_o.txt";
    std::string line;
    std::ofstream ostream;
    std::ifstream istream;
    istream.open(file_i, std::ios_base::in);
    ostream.open(file_o, std::ios_base::out);
    int counter = 0;
    while(std::getline(istream, line))
    {
        if (counter == 100)
            break;
        std::string new_line = std::to_string(counter)+" "+line;
        std::cout << (new_line.c_str()) << "\n";
        ostream << new_line.c_str();
        counter++;
    }
    istream.close();
    ostream.close();
#endif
#ifdef false
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
#endif
    const int size = 50;
    char array[size];
    array[5] = 'h';
    printf("%s", array);
    }  