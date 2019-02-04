#include <cstdlib>
#include <iostream>
#include <ctime>

const int size_i = 100;
const int size_j = 10;
const int size_k = 5;

int main() {
    int a[size_i][size_j][size_k][size_j];
    int tmp;
    int i, j, k;

    for (i = size_j-1; i >=0; i--) {
        for (j = 0; j < size_k; j++) {
            for (k = 0; k < size_i; k++) {
                a[k][i][j][0] = i*j*k*a[k][i][j-1][0];
            }
        }
    }

    return 0;
}
