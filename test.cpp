#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <fstream>

int main(int argc, char** argv)
{
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
}  