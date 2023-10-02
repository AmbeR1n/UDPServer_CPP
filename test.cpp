#include <iostream>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <string.h>
#include <algorithm>

int main()
{
    char text1[1000];
    const char* END = "<END>";
    strcpy(text1, "gjkialoshfdgbviujfdahbvdiujla");
    strcpy(text1, "");
    strcpy(text1, "<END>");
    std::cout << sizeof(int64_t) << "\n";
    return 0;
}