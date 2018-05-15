#include <cstdlib>
#include <iostream>
#include <ctime>

const int size = 100;

int matrix_get_mult_element_axb(int a[size][size], int b[size][size], int i, int j) {
    int tmp;
    int k;

    tmp = 0;
    for (k = 0; k < size; k++) {
        tmp += a[i][k]*b[k][j];
    }

    return tmp;
}

int main() {
    int i,j;

    int a[size][size];
    int b[size][size];
    int c[size][size];
   
    std::srand(std::time(nullptr));

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            a[i][j] = std::rand() % 10; 
            b[i][j] = std::rand() % 10; 
        }
    }

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            c[i][j] = matrix_get_mult_element_axb(a,b,i,j);
        }
    }

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            std::cout << c[i][j] << " ";
        }
        std::cout << "\n";
    }

    return 0;
}
