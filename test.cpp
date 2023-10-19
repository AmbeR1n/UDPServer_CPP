#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <fstream>

int main(int argc, char** argv)
{
    int arr1[10];
    int* arr2 = new int[12];
    for (int i = 0; i < 15; i++)
        printf("%d\n", arr2[i]);
    return 0;
    delete[] arr2;
}