#include <iostream>

using namespace std;

int main() 
{
    int* a = new int[100];

    for (int i=0; i<100; i++)
        {
            a[i] = i;
            cout << a[i] << " ";
        }
}