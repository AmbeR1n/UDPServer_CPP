#ifndef FILE_H
#define FILE_H
#include <fstream>
#include <filesystem>

struct File
{
    std::ifstream stream;
    long size;
    char* path;
    File() {size = 0; path = (char*)"";};
    File(char* _path) 
    {
        path = _path;
        stream.open(_path);
        size = std::filesystem::file_size(path);
    }
};
#endif