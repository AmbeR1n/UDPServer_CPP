#include <iostream>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include <complex>

int main()
{
    char text1[1000];
    const char* END = "<END>";
    strcpy(text1, "gjkialoshfdgbviujfdahbvdiujla");
    strcpy(text1, "");
    strcpy(text1, "<END>");
    float r = 0.5;
    float i = 0.5;
    std::complex a(r, i);
    std::cout << sizeof(a) << "\n";
    return 0;
}