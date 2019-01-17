#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

typedef struct Node {
    int array[100];
    unsigned int checksum[100];
} node_t;

int main() {
    
    node_t a[100];

    for (unsigned int i = 0; i < 100; i++) {
        a[0].array[i] = i;
        a[0].checksum[i] = i % 10;
    }

    return 0;
}
