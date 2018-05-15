#include <cstdlib>
#include <iostream>
#include <ctime>

const int size = 100;

int main() {
    int i,j;

    int a[size][size];
    int b[size][size];
    int c[size][size];
   
    std::srand(std::time(nullptr));

    // parallelisible
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            a[i][j] = std::rand() % 10; 
            b[i][j] = std::rand() % 10; 
        }
    }

    // parallelisible
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }

    return 0;
}
