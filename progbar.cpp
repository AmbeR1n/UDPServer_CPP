#include <iostream>
#include <unistd.h>
using namespace std;
  
// Function to creating loading bar
void loadingBar()
{
  
    // Initialize char for printing
    // loading bar
    char a = 45, b = 219;

    printf("Loading... \n");
  
    // Print initial loading bar
    for (int i = 0; i < 26; i++)
        cout << a << flush;
  
    // Set the cursor again starting
    // point of loading bar
    printf("\r");
  
    // Print loading bar progress
    for (int i = 0; i < 26; i++) {
        fputs("\u25A0", stdout);
        fflush(stdout);
        
        // Sleep for 1 second
        usleep(100000);
    }
}
  
// Driver Code
int main()
{
    // Function Call
    loadingBar();
    printf("\n");
    return 0;
}