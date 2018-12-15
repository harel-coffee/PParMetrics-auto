#include <cstdlib>
#include <ctime>
#include <cmath>

#include <iostream>
using namespace std;

static const unsigned int a_hsize = 1000;
static const unsigned int a_vsize = 100;
static const unsigned int b_hsize = 3000;
static const unsigned int b_vsize = a_hsize;
static const unsigned int c_hsize = b_hsize;
static const unsigned int c_vsize = a_vsize;

static const unsigned int range = 10;
static const unsigned int experiments_num = 5;

void swap(unsigned int& a, unsigned int& b) {
    unsigned int tmp;

    tmp = a;
    a = b;
    b = tmp;
}

unsigned int get_matrix_value(unsigned int** a, unsigned int vsize, unsigned int hsize) {
    unsigned int total = 0;
    unsigned int i, j;

    unsigned int** b;
    b = new unsigned int* [vsize];
    for (i = 0; i < vsize; i++) {
        b[i] = new unsigned int [hsize]; 
    }

    for (i = 0; i < vsize; i++) {
        for (j = 0; j < hsize; j++) {
            b[i][j] = a[i][j];
        }
    }

    for (i = 0; i < vsize; i++) {
        for (j = 1; j < hsize; j++) {
            b[i][j] += b[i][j-1];
        }
        total += b[i][j-1];
    }

    for (i = 0; i < vsize; i++) {
        delete [] b[i]; 
    }
    delete [] b;

    return total;
}

int main() {
    unsigned int experiments[experiments_num];
    unsigned int tmp;
    
    std::srand(std::time(nullptr));
    
    // Allocate memory for matrices

    unsigned int** a;
    a = new unsigned int* [a_vsize];
    for (unsigned int i = 0; i < a_vsize; i++) {
        a[i] = new unsigned int [a_hsize]; 
    }

    unsigned int** b;
    b = new unsigned int* [b_vsize];
    for (unsigned int i = 0; i < b_vsize; i++) {
        b[i] = new unsigned int [b_hsize]; 
    }

    unsigned int** c;
    c = new unsigned int* [c_vsize];
    for (unsigned int i = 0; i < c_vsize; i++) {
        c[i] = new unsigned int [c_hsize]; 
    }

    for (unsigned int expn = 0; expn < experiments_num; expn++)
    {
        // Fill matrices with randomly generated numbers
        
        for (unsigned int i = 0; i < a_vsize; i++) {
            for (unsigned int j = 0; j < a_hsize; j++) {
                a[i][j] = rand() % range;
            }
        }
    
        for (unsigned int i = 0; i < b_vsize; i++) {
            for (unsigned int j = 0; j < b_hsize; j++) {
                b[i][j] = rand() % range;
            }
        }

        for (unsigned int i = 0; i < c_vsize; i++) {
            for (unsigned int j = 0; j < c_hsize; j++) {
                for (unsigned int k = 0; k < a_hsize; k++) {
                    c[i][j] = a[i][k]*b[k][j];
                }
                c[i][j] += 1;
            }
        }

        experiments[expn] = get_matrix_value(c, c_vsize, c_hsize);
        
        for (unsigned int i = 0; i < c_vsize; i++) {
            for (unsigned int j = 0; j < c_hsize/2; j++) {
                tmp = c[i][j];
                c[i][j] = c[i][c_hsize-j-1];
                c[i][c_hsize-j-1] = tmp;
            }
        }
    
        for (unsigned int j = 0; j < c_hsize; j++) {
            for (unsigned int i = 0; i < c_vsize/2; i++) {
                swap(c[i][j], c[c_vsize-1-i][j]);
            }
        }

        if (experiments[expn] == get_matrix_value(c, c_vsize, c_hsize)) {
            cout << "Experiment " << expn << " passed!" << endl;
        } else {
            cout << "Experiment " << expn << " failed!" << endl;
        }
    }

    for (unsigned int i = 0; i < a_vsize; i++) {
        delete [] a[i]; 
    }
    delete [] a;

    for (unsigned int i = 0; i < b_vsize; i++) {
        delete [] b[i]; 
    }
    delete [] b;

    for (unsigned int i = 0; i < c_vsize; i++) {
        delete [] c[i]; 
    }
    delete [] c;

    return 0;
}
