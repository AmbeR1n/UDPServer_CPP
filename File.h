#ifndef FILE_H
#define FILE_H
#include <fstream>
#include <filesystem>

struct File
{
    std::ifstream stream;
    int size;
    std::filesystem::path path;
    File() {size = 0; path = (char*)"";};
    File(char* _path) 
    {
        path = std::filesystem::path(_path);
        stream.open(_path);
        size = std::filesystem::file_size(path);
    }
};
#endif