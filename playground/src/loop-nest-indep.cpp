#include <cstdlib>
#include <iostream>
#include <ctime>

const int size_i = 100;
const int size_j = 10;
const int size_k = 5;

int main() {
    int a[size_i][size_j][size_k];
    int tmp;

    // parallelisible
    for (int i = 0; i < size_i; i++) {
        // parallelisible
        for (int j = 0; j < size_j; j++) {
            // parallelisible
            for (int k = 1; k < size_k; k++) {
                tmp = (i+j+k)/3;
                a[i][j][k] = a[i][j][k] + tmp; 
            }
        }
    }

    return 0;
}
