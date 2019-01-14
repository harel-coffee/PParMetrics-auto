#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

int main() {

    int a[100];
    int n = 100;
    int tmp;

    for (int i = 0; i < n; i++) {
        a[i] = a[i-1];
    }

    return 0;
}
