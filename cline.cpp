#include <iostream>
using namespace std;
 
//    defining main with arguments
int main(int argc, char** argv)
{   
    double s = 93;
    double e = 100;
    for (double i = s; i < e; i+=0.5)
        cout << (int)i/5 << endl;
    return 0;
}