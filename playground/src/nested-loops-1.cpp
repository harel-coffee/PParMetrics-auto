#include <cstdlib>
#include <iostream>
#include <ctime>

const int size = 100;

int main() {
    int a[size][size];
    int b[size][size];
    int c[size][size];
   
    std::srand(std::time(nullptr));

    // parallelisible
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            a[i][j] = std::rand() % 10; 
            b[i][j] = std::rand() % 10; 
        }
    }

    // parallelisible
    for (int i = 1; i < size; i++) {
        for (int j = 0; j < size; j++) {
            c[i-1][j+3] = a[i][j+2] + b[i][j];
        }
    }

    return 0;
}
