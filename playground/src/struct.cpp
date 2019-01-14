#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

int func(int *a, int *b, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = 0;
        b[i] = 1;
    }
}

int main() {
    
    int a[100];
    int b[100];

    func(a,b,100);

    return 0;
}
