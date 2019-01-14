#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

int main() {
    
    int nn;
    int n = 100;
    int a[100];

    nn = 1;
    do {
        nn = nn * 2;
        a[nn] = nn;
        for (int i = nn/2; i<nn-1; i++) {
            a[i] = nn-1;
        }
    } while (nn < n);

    return 0;
}
