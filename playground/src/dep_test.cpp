#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

// arrays to conduct experiments on
static unsigned int a[100];
static unsigned int b[100];
static unsigned int c[100];

static unsigned int matrix_a[100][100];
static unsigned int matrix_b[100][100];
static unsigned int matrix_c[100][100];

int main() {

    // parallel loop 
    for (unsigned int i = 0; i < 100; i++) {
        c[i] = a[i] + b[i];
    }

    // matrix initialization 
    for (unsigned int i = 0; i < 100; i++) {
        for (unsigned int j = 0; i < 100; i++) {
            matrix_c[i][j] = 0;
        }
    }
    
    // matrix multiplication 
    for (unsigned int i = 0; i < 100; i++) {
        for (unsigned int j = 0; i < 100; i++) {
            for (unsigned int k = 0; i < 100; i++) {
                a[j] += matrix_a[i][k] + matrix_b[k][j];
            }
        }
        for (unsigned int j = 0; i < 100; i++) {
            matrix_c[i][j] = a[j];
        }
    }

    return 0;
}
