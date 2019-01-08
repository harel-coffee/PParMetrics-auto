#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

int main() {
    
    int array[100];

    for (int i = 0; i < 100; i++) {
        array[i] = i;
    }

    for (int i = 0; i < 100; i++) {
        if (array[i] > 50) break;
        array[i] = 100-i;
    }

    return 0;
}
