#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

#define LX1 100

const unsigned int n=100;

int main() {
    
    int a[n];
    unsigned long product = 1;

    for(int i=0; i<=n; i++) {
        while(a[i]>0) {
            product *= i;
            if ( product>500000 ) return product;
            a[i]--;
        }
    }

    return 0;
}
